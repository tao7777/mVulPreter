static void cmdloop(void)
{
    int c;
    int usinguid, havepartition, havenamespace, recursive;
    static struct buf tag, cmd, arg1, arg2, arg3;
    char *p, shut[MAX_MAILBOX_PATH+1], cmdname[100];
    const char *err;
    const char * commandmintimer;
    double commandmintimerd = 0.0;
    struct sync_reserve_list *reserve_list =
        sync_reserve_list_create(SYNC_MESSAGE_LIST_HASH_SIZE);
    struct applepushserviceargs applepushserviceargs;

    prot_printf(imapd_out, "* OK [CAPABILITY ");
    capa_response(CAPA_PREAUTH);
    prot_printf(imapd_out, "]");
    if (config_serverinfo) prot_printf(imapd_out, " %s", config_servername);
    if (config_serverinfo == IMAP_ENUM_SERVERINFO_ON) {
        prot_printf(imapd_out, " Cyrus IMAP %s", cyrus_version());
    }
    prot_printf(imapd_out, " server ready\r\n");

    /* clear cancelled flag if present before the next command */
    cmd_cancelled();

    motd_file();

    /* Get command timer logging paramater. This string
     * is a time in seconds. Any command that takes >=
     * this time to execute is logged */
    commandmintimer = config_getstring(IMAPOPT_COMMANDMINTIMER);
    cmdtime_settimer(commandmintimer ? 1 : 0);
    if (commandmintimer) {
      commandmintimerd = atof(commandmintimer);
    }

    for (;;) {
        /* Release any held index */
        index_release(imapd_index);

        /* Flush any buffered output */
        prot_flush(imapd_out);
        if (backend_current) prot_flush(backend_current->out);

        /* command no longer running */
        proc_register(config_ident, imapd_clienthost, imapd_userid, index_mboxname(imapd_index), NULL);

        /* Check for shutdown file */
        if ( !imapd_userisadmin && imapd_userid &&
             (shutdown_file(shut, sizeof(shut)) ||
              userdeny(imapd_userid, config_ident, shut, sizeof(shut)))) {
            for (p = shut; *p == '['; p++); /* can't have [ be first char */
            prot_printf(imapd_out, "* BYE [ALERT] %s\r\n", p);
            telemetry_rusage(imapd_userid);
            shut_down(0);
        }

        signals_poll();

        if (!proxy_check_input(protin, imapd_in, imapd_out,
                               backend_current ? backend_current->in : NULL,
                               NULL, 0)) {
            /* No input from client */
            continue;
        }

        /* Parse tag */
        c = getword(imapd_in, &tag);
        if (c == EOF) {
            if ((err = prot_error(imapd_in))!=NULL
                && strcmp(err, PROT_EOF_STRING)) {
                syslog(LOG_WARNING, "%s, closing connection", err);
                prot_printf(imapd_out, "* BYE %s\r\n", err);
            }
            goto done;
        }
        if (c != ' ' || !imparse_isatom(tag.s) || (tag.s[0] == '*' && !tag.s[1])) {
            prot_printf(imapd_out, "* BAD Invalid tag\r\n");
            eatline(imapd_in, c);
            continue;
        }

        /* Parse command name */
        c = getword(imapd_in, &cmd);
        if (!cmd.s[0]) {
            prot_printf(imapd_out, "%s BAD Null command\r\n", tag.s);
            eatline(imapd_in, c);
            continue;
        }
        lcase(cmd.s);
        xstrncpy(cmdname, cmd.s, 99);
        cmd.s[0] = toupper((unsigned char) cmd.s[0]);

        if (config_getswitch(IMAPOPT_CHATTY))
            syslog(LOG_NOTICE, "command: %s %s", tag.s, cmd.s);

        proc_register(config_ident, imapd_clienthost, imapd_userid, index_mboxname(imapd_index), cmd.s);

        /* if we need to force a kick, do so */
        if (referral_kick) {
            kick_mupdate();
            referral_kick = 0;
        }

        if (plaintextloginalert) {
            prot_printf(imapd_out, "* OK [ALERT] %s\r\n",
                        plaintextloginalert);
            plaintextloginalert = NULL;
        }

        /* Only Authenticate/Enable/Login/Logout/Noop/Capability/Id/Starttls
           allowed when not logged in */
        if (!imapd_userid && !strchr("AELNCIS", cmd.s[0])) goto nologin;

        /* Start command timer */
        cmdtime_starttimer();

        /* note that about half the commands (the common ones that don't
           hit the mailboxes file) now close the mailboxes file just in
           case it was open. */
        switch (cmd.s[0]) {
        case 'A':
            if (!strcmp(cmd.s, "Authenticate")) {
                int haveinitresp = 0;

                if (c != ' ') goto missingargs;
                c = getword(imapd_in, &arg1);
                if (!imparse_isatom(arg1.s)) {
                    prot_printf(imapd_out, "%s BAD Invalid authenticate mechanism\r\n", tag.s);
                    eatline(imapd_in, c);
                    continue;
                }
                if (c == ' ') {
                    haveinitresp = 1;
                    c = getword(imapd_in, &arg2);
                    if (c == EOF) goto missingargs;
                }
                if (c == '\r') c = prot_getc(imapd_in);
                if (c != '\n') goto extraargs;

                if (imapd_userid) {
                    prot_printf(imapd_out, "%s BAD Already authenticated\r\n", tag.s);
                    continue;
                }
                cmd_authenticate(tag.s, arg1.s, haveinitresp ? arg2.s : NULL);

                snmp_increment(AUTHENTICATE_COUNT, 1);
            }
            else if (!imapd_userid) goto nologin;
            else if (!strcmp(cmd.s, "Append")) {
                if (c != ' ') goto missingargs;
                c = getastring(imapd_in, imapd_out, &arg1);
                if (c != ' ') goto missingargs;

                cmd_append(tag.s, arg1.s, NULL);

                snmp_increment(APPEND_COUNT, 1);
            }
            else goto badcmd;
            break;

        case 'C':
            if (!strcmp(cmd.s, "Capability")) {
                if (c == '\r') c = prot_getc(imapd_in);
                if (c != '\n') goto extraargs;
                cmd_capability(tag.s);

                snmp_increment(CAPABILITY_COUNT, 1);
            }
            else if (!imapd_userid) goto nologin;
#ifdef HAVE_ZLIB
            else if (!strcmp(cmd.s, "Compress")) {
                if (c != ' ') goto missingargs;
                c = getword(imapd_in, &arg1);
                if (c == EOF) goto missingargs;
                if (c == '\r') c = prot_getc(imapd_in);
                if (c != '\n') goto extraargs;

                cmd_compress(tag.s, arg1.s);

                snmp_increment(COMPRESS_COUNT, 1);
            }
#endif /* HAVE_ZLIB */
            else if (!strcmp(cmd.s, "Check")) {
                if (!imapd_index && !backend_current) goto nomailbox;
                if (c == '\r') c = prot_getc(imapd_in);
                if (c != '\n') goto extraargs;

                cmd_noop(tag.s, cmd.s);

                snmp_increment(CHECK_COUNT, 1);
            }
            else if (!strcmp(cmd.s, "Copy")) {
                if (!imapd_index && !backend_current) goto nomailbox;
                usinguid = 0;
                if (c != ' ') goto missingargs;
            copy:
                c = getword(imapd_in, &arg1);
                if (c == '\r') goto missingargs;
                if (c != ' ' || !imparse_issequence(arg1.s)) goto badsequence;
                c = getastring(imapd_in, imapd_out, &arg2);
                if (c == EOF) goto missingargs;
                if (c == '\r') c = prot_getc(imapd_in);
                if (c != '\n') goto extraargs;

                cmd_copy(tag.s, arg1.s, arg2.s, usinguid, /*ismove*/0);

                snmp_increment(COPY_COUNT, 1);
            }
            else if (!strcmp(cmd.s, "Create")) {
                struct dlist *extargs = NULL;

                if (c != ' ') goto missingargs;
                c = getastring(imapd_in, imapd_out, &arg1);
                if (c == EOF) goto missingargs;
                if (c == ' ') {
                    c = parsecreateargs(&extargs);
                    if (c == EOF) goto badpartition;
                }
                if (c == '\r') c = prot_getc(imapd_in);
                if (c != '\n') goto extraargs;
                cmd_create(tag.s, arg1.s, extargs, 0);
                dlist_free(&extargs);

                snmp_increment(CREATE_COUNT, 1);
            }
            else if (!strcmp(cmd.s, "Close")) {
                if (!imapd_index && !backend_current) goto nomailbox;
                if (c == '\r') c = prot_getc(imapd_in);
                if (c != '\n') goto extraargs;

                cmd_close(tag.s, cmd.s);

                snmp_increment(CLOSE_COUNT, 1);
            }
            else goto badcmd;
            break;

        case 'D':
            if (!strcmp(cmd.s, "Delete")) {
                if (c != ' ') goto missingargs;
                c = getastring(imapd_in, imapd_out, &arg1);
                if (c == EOF) goto missingargs;
                if (c == '\r') c = prot_getc(imapd_in);
                if (c != '\n') goto extraargs;
                cmd_delete(tag.s, arg1.s, 0, 0);

                snmp_increment(DELETE_COUNT, 1);
            }
            else if (!strcmp(cmd.s, "Deleteacl")) {
                if (c != ' ') goto missingargs;
                c = getastring(imapd_in, imapd_out, &arg1);
                if (c != ' ') goto missingargs;
                c = getastring(imapd_in, imapd_out, &arg2);
                if (c == EOF) goto missingargs;
                if (c == '\r') c = prot_getc(imapd_in);
                if (c != '\n') goto extraargs;
                cmd_setacl(tag.s, arg1.s, arg2.s, NULL);

                snmp_increment(DELETEACL_COUNT, 1);
            }
            else if (!strcmp(cmd.s, "Dump")) {
                int uid_start = 0;

                if(c != ' ') goto missingargs;
                c = getastring(imapd_in, imapd_out, &arg1);
                if(c == ' ') {
                    c = getastring(imapd_in, imapd_out, &arg2);
                    if(!imparse_isnumber(arg2.s)) goto extraargs;
                    uid_start = atoi(arg2.s);
                }

                if(c == '\r') c = prot_getc(imapd_in);
                if(c != '\n') goto extraargs;

                cmd_dump(tag.s, arg1.s, uid_start);
            /*  snmp_increment(DUMP_COUNT, 1);*/
            }
            else goto badcmd;
            break;

        case 'E':
            if (!imapd_userid) goto nologin;
            else if (!strcmp(cmd.s, "Enable")) {
                if (c != ' ') goto missingargs;

                cmd_enable(tag.s);
            }
            else if (!strcmp(cmd.s, "Expunge")) {
                if (!imapd_index && !backend_current) goto nomailbox;
                if (c == '\r') c = prot_getc(imapd_in);
                if (c != '\n') goto extraargs;

                cmd_expunge(tag.s, 0);

                snmp_increment(EXPUNGE_COUNT, 1);
            }
            else if (!strcmp(cmd.s, "Examine")) {
                if (c != ' ') goto missingargs;
                c = getastring(imapd_in, imapd_out, &arg1);
                if (c == EOF) goto missingargs;
                prot_ungetc(c, imapd_in);

                cmd_select(tag.s, cmd.s, arg1.s);

                snmp_increment(EXAMINE_COUNT, 1);
            }
            else goto badcmd;
            break;

        case 'F':
            if (!strcmp(cmd.s, "Fetch")) {
                if (!imapd_index && !backend_current) goto nomailbox;
                usinguid = 0;
                if (c != ' ') goto missingargs;
            fetch:
                c = getword(imapd_in, &arg1);
                if (c == '\r') goto missingargs;
                if (c != ' ' || !imparse_issequence(arg1.s)) goto badsequence;

                cmd_fetch(tag.s, arg1.s, usinguid);

                snmp_increment(FETCH_COUNT, 1);
            }
            else goto badcmd;
            break;

        case 'G':
            if (!strcmp(cmd.s, "Getacl")) {
                if (c != ' ') goto missingargs;
                c = getastring(imapd_in, imapd_out, &arg1);
                if (c == EOF) goto missingargs;
                if (c == '\r') c = prot_getc(imapd_in);
                if (c != '\n') goto extraargs;
                cmd_getacl(tag.s, arg1.s);

                snmp_increment(GETACL_COUNT, 1);
            }
            else if (!strcmp(cmd.s, "Getannotation")) {
                if (c != ' ') goto missingargs;
                c = getastring(imapd_in, imapd_out, &arg1);
                if (c != ' ') goto missingargs;

                cmd_getannotation(tag.s, arg1.s);

                snmp_increment(GETANNOTATION_COUNT, 1);
            }
            else if (!strcmp(cmd.s, "Getmetadata")) {
                if (c != ' ') goto missingargs;

                cmd_getmetadata(tag.s);

                snmp_increment(GETANNOTATION_COUNT, 1);
            }
            else if (!strcmp(cmd.s, "Getquota")) {
                if (c != ' ') goto missingargs;
                c = getastring(imapd_in, imapd_out, &arg1);
                if (c == EOF) goto missingargs;
                if (c == '\r') c = prot_getc(imapd_in);
                if (c != '\n') goto extraargs;
                cmd_getquota(tag.s, arg1.s);

                snmp_increment(GETQUOTA_COUNT, 1);
            }
            else if (!strcmp(cmd.s, "Getquotaroot")) {
                if (c != ' ') goto missingargs;
                c = getastring(imapd_in, imapd_out, &arg1);
                if (c == EOF) goto missingargs;
                if (c == '\r') c = prot_getc(imapd_in);
                if (c != '\n') goto extraargs;
                cmd_getquotaroot(tag.s, arg1.s);

                snmp_increment(GETQUOTAROOT_COUNT, 1);
            }
#ifdef HAVE_SSL
            else if (!strcmp(cmd.s, "Genurlauth")) {
                if (c != ' ') goto missingargs;

                cmd_genurlauth(tag.s);
            /*  snmp_increment(GENURLAUTH_COUNT, 1);*/
            }
#endif
            else goto badcmd;
            break;

        case 'I':
            if (!strcmp(cmd.s, "Id")) {
                if (c != ' ') goto missingargs;
                cmd_id(tag.s);

                snmp_increment(ID_COUNT, 1);
            }
            else if (!imapd_userid) goto nologin;
            else if (!strcmp(cmd.s, "Idle") && idle_enabled()) {
                if (c == '\r') c = prot_getc(imapd_in);
                if (c != '\n') goto extraargs;
                cmd_idle(tag.s);

                snmp_increment(IDLE_COUNT, 1);
            }
            else goto badcmd;
            break;

        case 'L':
            if (!strcmp(cmd.s, "Login")) {
                if (c != ' ') goto missingargs;
                c = getastring(imapd_in, imapd_out, &arg1);
                if(c != ' ') goto missingargs;

                cmd_login(tag.s, arg1.s);

                snmp_increment(LOGIN_COUNT, 1);
            }
            else if (!strcmp(cmd.s, "Logout")) {
                if (c == '\r') c = prot_getc(imapd_in);
                if (c != '\n') goto extraargs;

                snmp_increment(LOGOUT_COUNT, 1);

                /* force any responses from our selected backend */
                if (backend_current) imapd_check(NULL, 0);

                prot_printf(imapd_out, "* BYE %s\r\n",
                            error_message(IMAP_BYE_LOGOUT));
                prot_printf(imapd_out, "%s OK %s\r\n", tag.s,
                            error_message(IMAP_OK_COMPLETED));

                if (imapd_userid && *imapd_userid) {
                    telemetry_rusage(imapd_userid);
                }

                goto done;
            }
            else if (!imapd_userid) goto nologin;
            else if (!strcmp(cmd.s, "List")) {
                struct listargs listargs;

                if (c != ' ') goto missingargs;

                memset(&listargs, 0, sizeof(struct listargs));
                listargs.ret = LIST_RET_CHILDREN;
                getlistargs(tag.s, &listargs);
                if (listargs.pat.count) cmd_list(tag.s, &listargs);

                snmp_increment(LIST_COUNT, 1);
            }
            else if (!strcmp(cmd.s, "Lsub")) {
                struct listargs listargs;

                c = getastring(imapd_in, imapd_out, &arg1);
                if (c != ' ') goto missingargs;
                c = getastring(imapd_in, imapd_out, &arg2);
                if (c == '\r') c = prot_getc(imapd_in);
                if (c != '\n') goto extraargs;

                memset(&listargs, 0, sizeof(struct listargs));
                listargs.cmd = LIST_CMD_LSUB;
                listargs.sel = LIST_SEL_SUBSCRIBED;
                if (!strcasecmpsafe(imapd_magicplus, "+dav"))
                    listargs.sel |= LIST_SEL_DAV;
                listargs.ref = arg1.s;
                strarray_append(&listargs.pat, arg2.s);

                cmd_list(tag.s, &listargs);

                snmp_increment(LSUB_COUNT, 1);
            }
            else if (!strcmp(cmd.s, "Listrights")) {
                c = getastring(imapd_in, imapd_out, &arg1);
                if (c != ' ') goto missingargs;
                c = getastring(imapd_in, imapd_out, &arg2);
                if (c == '\r') c = prot_getc(imapd_in);
                if (c != '\n') goto extraargs;
                cmd_listrights(tag.s, arg1.s, arg2.s);

                snmp_increment(LISTRIGHTS_COUNT, 1);
            }
            else if (!strcmp(cmd.s, "Localappend")) {
                /* create a local-only mailbox */
                if (c != ' ') goto missingargs;
                c = getastring(imapd_in, imapd_out, &arg1);
                if (c != ' ') goto missingargs;
                c = getastring(imapd_in, imapd_out, &arg2);
                if (c != ' ') goto missingargs;

                cmd_append(tag.s, arg1.s, *arg2.s ? arg2.s : NULL);

                snmp_increment(APPEND_COUNT, 1);
            }
            else if (!strcmp(cmd.s, "Localcreate")) {
                /* create a local-only mailbox */
                struct dlist *extargs = NULL;

                if (c != ' ') goto missingargs;
                c = getastring(imapd_in, imapd_out, &arg1);
                if (c == EOF) goto missingargs;
                if (c == ' ') {
                    c = parsecreateargs(&extargs);
                    if (c == EOF) goto badpartition;
                }
                if (c == '\r') c = prot_getc(imapd_in);
                if (c != '\n') goto extraargs;
                cmd_create(tag.s, arg1.s, extargs, 1);
                dlist_free(&extargs);

                /* xxxx snmp_increment(CREATE_COUNT, 1); */
            }
            else if (!strcmp(cmd.s, "Localdelete")) {
                /* delete a mailbox locally only */
                if (c != ' ') goto missingargs;
                c = getastring(imapd_in, imapd_out, &arg1);
                if (c == EOF) goto missingargs;
                if (c == '\r') c = prot_getc(imapd_in);
                if (c != '\n') goto extraargs;
                cmd_delete(tag.s, arg1.s, 1, 1);

                /* xxxx snmp_increment(DELETE_COUNT, 1); */
            }
            else goto badcmd;
            break;

        case 'M':
            if (!strcmp(cmd.s, "Myrights")) {
                if (c != ' ') goto missingargs;
                c = getastring(imapd_in, imapd_out, &arg1);
                if (c == EOF) goto missingargs;
                if (c == '\r') c = prot_getc(imapd_in);
                if (c != '\n') goto extraargs;
                cmd_myrights(tag.s, arg1.s);

                /* xxxx snmp_increment(MYRIGHTS_COUNT, 1); */
            }
            else if (!strcmp(cmd.s, "Mupdatepush")) {
                if (c != ' ') goto missingargs;
                c = getastring(imapd_in, imapd_out, &arg1);
                if(c == EOF) goto missingargs;
                if(c == '\r') c = prot_getc(imapd_in);
                if(c != '\n') goto extraargs;
                cmd_mupdatepush(tag.s, arg1.s);

                /* xxxx snmp_increment(MUPDATEPUSH_COUNT, 1); */
            }
            else if (!strcmp(cmd.s, "Move")) {
                if (!imapd_index && !backend_current) goto nomailbox;
                usinguid = 0;
                if (c != ' ') goto missingargs;
            move:
                c = getword(imapd_in, &arg1);
                if (c == '\r') goto missingargs;
                if (c != ' ' || !imparse_issequence(arg1.s)) goto badsequence;
                c = getastring(imapd_in, imapd_out, &arg2);
                if (c == EOF) goto missingargs;
                if (c == '\r') c = prot_getc(imapd_in);
                if (c != '\n') goto extraargs;

                cmd_copy(tag.s, arg1.s, arg2.s, usinguid, /*ismove*/1);

                snmp_increment(COPY_COUNT, 1);
            } else goto badcmd;
            break;

        case 'N':
            if (!strcmp(cmd.s, "Noop")) {
                if (c == '\r') c = prot_getc(imapd_in);
                if (c != '\n') goto extraargs;

                cmd_noop(tag.s, cmd.s);

                /* xxxx snmp_increment(NOOP_COUNT, 1); */
            }
            else if (!imapd_userid) goto nologin;
            else if (!strcmp(cmd.s, "Namespace")) {
                if (c == '\r') c = prot_getc(imapd_in);
                if (c != '\n') goto extraargs;
                cmd_namespace(tag.s);

                /* xxxx snmp_increment(NAMESPACE_COUNT, 1); */
            }
            else goto badcmd;
            break;

        case 'R':
            if (!strcmp(cmd.s, "Rename")) {
                havepartition = 0;
                if (c != ' ') goto missingargs;
                c = getastring(imapd_in, imapd_out, &arg1);
                if (c != ' ') goto missingargs;
                c = getastring(imapd_in, imapd_out, &arg2);
                if (c == EOF) goto missingargs;
                if (c == ' ') {
                    havepartition = 1;
                    c = getword(imapd_in, &arg3);
                    if (!imparse_isatom(arg3.s)) goto badpartition;
                }
                if (c == '\r') c = prot_getc(imapd_in);
                if (c != '\n') goto extraargs;
                cmd_rename(tag.s, arg1.s, arg2.s, havepartition ? arg3.s : 0);

                /* xxxx snmp_increment(RENAME_COUNT, 1); */
            } else if(!strcmp(cmd.s, "Reconstruct")) {
                recursive = 0;
                if (c != ' ') goto missingargs;
                c = getastring(imapd_in, imapd_out, &arg1);
                if(c == ' ') {
                    /* Optional RECURSEIVE argument */
                    c = getword(imapd_in, &arg2);
                    if(!imparse_isatom(arg2.s))
                        goto extraargs;
                    else if(!strcasecmp(arg2.s, "RECURSIVE"))
                        recursive = 1;
                    else
                        goto extraargs;
                }
                if(c == '\r') c = prot_getc(imapd_in);
                if(c != '\n') goto extraargs;
                cmd_reconstruct(tag.s, arg1.s, recursive);

                /* snmp_increment(RECONSTRUCT_COUNT, 1); */
            }
            else if (!strcmp(cmd.s, "Rlist")) {
                struct listargs listargs;

                c = getastring(imapd_in, imapd_out, &arg1);
                if (c != ' ') goto missingargs;
                c = getastring(imapd_in, imapd_out, &arg2);
                if (c == '\r') c = prot_getc(imapd_in);
                if (c != '\n') goto extraargs;

                memset(&listargs, 0, sizeof(struct listargs));
                listargs.sel = LIST_SEL_REMOTE;
                listargs.ret = LIST_RET_CHILDREN;
                listargs.ref = arg1.s;
                strarray_append(&listargs.pat, arg2.s);

                cmd_list(tag.s, &listargs);

/*              snmp_increment(LIST_COUNT, 1); */
            }
            else if (!strcmp(cmd.s, "Rlsub")) {
                struct listargs listargs;

                c = getastring(imapd_in, imapd_out, &arg1);
                if (c != ' ') goto missingargs;
                c = getastring(imapd_in, imapd_out, &arg2);
                if (c == '\r') c = prot_getc(imapd_in);
                if (c != '\n') goto extraargs;

                memset(&listargs, 0, sizeof(struct listargs));
                listargs.cmd = LIST_CMD_LSUB;
                listargs.sel = LIST_SEL_REMOTE | LIST_SEL_SUBSCRIBED;
                listargs.ref = arg1.s;
                strarray_append(&listargs.pat, arg2.s);

                cmd_list(tag.s, &listargs);

/*              snmp_increment(LSUB_COUNT, 1); */
            }
#ifdef HAVE_SSL
            else if (!strcmp(cmd.s, "Resetkey")) {
                int have_mbox = 0, have_mech = 0;

                if (c == ' ') {
                    have_mbox = 1;
                    c = getastring(imapd_in, imapd_out, &arg1);
                    if (c == EOF) goto missingargs;
                    if (c == ' ') {
                        have_mech = 1;
                        c = getword(imapd_in, &arg2);
                    }
                }

                if (c == '\r') c = prot_getc(imapd_in);
                if (c != '\n') goto extraargs;
                cmd_resetkey(tag.s, have_mbox ? arg1.s : 0,
                             have_mech ? arg2.s : 0);
            /*  snmp_increment(RESETKEY_COUNT, 1);*/
            }
#endif
            else goto badcmd;
            break;

        case 'S':
            if (!strcmp(cmd.s, "Starttls")) {
                if (!tls_enabled()) {
                    /* we don't support starttls */
                    goto badcmd;
                }

                if (c == '\r') c = prot_getc(imapd_in);
                if (c != '\n') goto extraargs;

                /* XXX  discard any input pipelined after STARTTLS */
                prot_flush(imapd_in);

                /* if we've already done SASL fail */
                if (imapd_userid != NULL) {
                    prot_printf(imapd_out,
               "%s BAD Can't Starttls after authentication\r\n", tag.s);
                    continue;
                }

                /* if we've already done COMPRESS fail */
                if (imapd_compress_done == 1) {
                    prot_printf(imapd_out,
               "%s BAD Can't Starttls after Compress\r\n", tag.s);
                    continue;
                }

                /* check if already did a successful tls */
                if (imapd_starttls_done == 1) {
                    prot_printf(imapd_out,
                                "%s BAD Already did a successful Starttls\r\n",
                                tag.s);
                    continue;
                }
                cmd_starttls(tag.s, 0);

                snmp_increment(STARTTLS_COUNT, 1);
                continue;
            }
            if (!imapd_userid) {
                goto nologin;
            } else if (!strcmp(cmd.s, "Store")) {
                if (!imapd_index && !backend_current) goto nomailbox;
                usinguid = 0;
                if (c != ' ') goto missingargs;
            store:
                c = getword(imapd_in, &arg1);
                if (c != ' ' || !imparse_issequence(arg1.s)) goto badsequence;

                cmd_store(tag.s, arg1.s, usinguid);

                snmp_increment(STORE_COUNT, 1);
            }
            else if (!strcmp(cmd.s, "Select")) {
                if (c != ' ') goto missingargs;
                c = getastring(imapd_in, imapd_out, &arg1);
                if (c == EOF) goto missingargs;
                prot_ungetc(c, imapd_in);

                cmd_select(tag.s, cmd.s, arg1.s);

                snmp_increment(SELECT_COUNT, 1);
            }
            else if (!strcmp(cmd.s, "Search")) {
                if (!imapd_index && !backend_current) goto nomailbox;
                usinguid = 0;
                if (c != ' ') goto missingargs;
            search:

                cmd_search(tag.s, usinguid);

                snmp_increment(SEARCH_COUNT, 1);
            }
            else if (!strcmp(cmd.s, "Subscribe")) {
                if (c != ' ') goto missingargs;
                havenamespace = 0;
                c = getastring(imapd_in, imapd_out, &arg1);
                if (c == ' ') {
                    havenamespace = 1;
                    c = getastring(imapd_in, imapd_out, &arg2);
                }
                if (c == EOF) goto missingargs;
                if (c == '\r') c = prot_getc(imapd_in);
                if (c != '\n') goto extraargs;
                if (havenamespace) {
                    cmd_changesub(tag.s, arg1.s, arg2.s, 1);
                }
                else {
                    cmd_changesub(tag.s, (char *)0, arg1.s, 1);
                }
                snmp_increment(SUBSCRIBE_COUNT, 1);
            }
            else if (!strcmp(cmd.s, "Setacl")) {
                if (c != ' ') goto missingargs;
                c = getastring(imapd_in, imapd_out, &arg1);
                if (c != ' ') goto missingargs;
                c = getastring(imapd_in, imapd_out, &arg2);
                if (c != ' ') goto missingargs;
                c = getastring(imapd_in, imapd_out, &arg3);
                if (c == EOF) goto missingargs;
                if (c == '\r') c = prot_getc(imapd_in);
                if (c != '\n') goto extraargs;
                cmd_setacl(tag.s, arg1.s, arg2.s, arg3.s);

                snmp_increment(SETACL_COUNT, 1);
            }
            else if (!strcmp(cmd.s, "Setannotation")) {
                if (c != ' ') goto missingargs;
                c = getastring(imapd_in, imapd_out, &arg1);
                if (c != ' ') goto missingargs;

                cmd_setannotation(tag.s, arg1.s);

                snmp_increment(SETANNOTATION_COUNT, 1);
            }
            else if (!strcmp(cmd.s, "Setmetadata")) {
                if (c != ' ') goto missingargs;
                c = getastring(imapd_in, imapd_out, &arg1);
                if (c != ' ') goto missingargs;

                cmd_setmetadata(tag.s, arg1.s);

                snmp_increment(SETANNOTATION_COUNT, 1);
            }
            else if (!strcmp(cmd.s, "Setquota")) {
                if (c != ' ') goto missingargs;
                c = getastring(imapd_in, imapd_out, &arg1);
                if (c != ' ') goto missingargs;
                cmd_setquota(tag.s, arg1.s);

                snmp_increment(SETQUOTA_COUNT, 1);
            }
            else if (!strcmp(cmd.s, "Sort")) {
                if (!imapd_index && !backend_current) goto nomailbox;
                usinguid = 0;
                if (c != ' ') goto missingargs;
            sort:
                cmd_sort(tag.s, usinguid);

                snmp_increment(SORT_COUNT, 1);
            }
            else if (!strcmp(cmd.s, "Status")) {
                if (c != ' ') goto missingargs;
                c = getastring(imapd_in, imapd_out, &arg1);
                if (c != ' ') goto missingargs;
                cmd_status(tag.s, arg1.s);

                snmp_increment(STATUS_COUNT, 1);
            }
            else if (!strcmp(cmd.s, "Scan")) {
                struct listargs listargs;

                c = getastring(imapd_in, imapd_out, &arg1);
                if (c != ' ') goto missingargs;
                c = getastring(imapd_in, imapd_out, &arg2);
                if (c != ' ') goto missingargs;
                c = getastring(imapd_in, imapd_out, &arg3);
                if (c == '\r') c = prot_getc(imapd_in);
                if (c != '\n') goto extraargs;

                memset(&listargs, 0, sizeof(struct listargs));
                listargs.ref = arg1.s;
                strarray_append(&listargs.pat, arg2.s);
                listargs.scan = arg3.s;

                cmd_list(tag.s, &listargs);

                 snmp_increment(SCAN_COUNT, 1);
             }
             else if (!strcmp(cmd.s, "Syncapply")) {
                if (!imapd_userisadmin) goto badcmd;

                 struct dlist *kl = sync_parseline(imapd_in);
 
                 if (kl) {
                    cmd_syncapply(tag.s, kl, reserve_list);
                    dlist_free(&kl);
                }
                 else goto extraargs;
             }
             else if (!strcmp(cmd.s, "Syncget")) {
                if (!imapd_userisadmin) goto badcmd;

                 struct dlist *kl = sync_parseline(imapd_in);
 
                 if (kl) {
                    cmd_syncget(tag.s, kl);
                    dlist_free(&kl);
                }
                 else goto extraargs;
             }
             else if (!strcmp(cmd.s, "Syncrestart")) {
                if (!imapd_userisadmin) goto badcmd;

                 if (c == '\r') c = prot_getc(imapd_in);
                 if (c != '\n') goto extraargs;
 
                 /* just clear the GUID cache */
                 cmd_syncrestart(tag.s, &reserve_list, 1);
             }
             else if (!strcmp(cmd.s, "Syncrestore")) {
                if (!imapd_userisadmin) goto badcmd;

                 struct dlist *kl = sync_parseline(imapd_in);
 
                 if (kl) {
                    cmd_syncrestore(tag.s, kl, reserve_list);
                    dlist_free(&kl);
                }
                else goto extraargs;
            }
            else goto badcmd;
            break;

        case 'T':
            if (!strcmp(cmd.s, "Thread")) {
                if (!imapd_index && !backend_current) goto nomailbox;
                usinguid = 0;
                if (c != ' ') goto missingargs;
            thread:
                cmd_thread(tag.s, usinguid);

                snmp_increment(THREAD_COUNT, 1);
            }
            else goto badcmd;
            break;

        case 'U':
            if (!strcmp(cmd.s, "Uid")) {
                if (!imapd_index && !backend_current) goto nomailbox;
                usinguid = 1;
                if (c != ' ') goto missingargs;
                c = getword(imapd_in, &arg1);
                if (c != ' ') goto missingargs;
                lcase(arg1.s);
                xstrncpy(cmdname, arg1.s, 99);
                if (!strcmp(arg1.s, "fetch")) {
                    goto fetch;
                }
                else if (!strcmp(arg1.s, "store")) {
                    goto store;
                }
                else if (!strcmp(arg1.s, "search")) {
                    goto search;
                }
                else if (!strcmp(arg1.s, "sort")) {
                    goto sort;
                }
                else if (!strcmp(arg1.s, "thread")) {
                    goto thread;
                }
                else if (!strcmp(arg1.s, "copy")) {
                    goto copy;
                }
                else if (!strcmp(arg1.s, "move")) {
                    goto move;
                }
                else if (!strcmp(arg1.s, "xmove")) {
                    goto move;
                }
                else if (!strcmp(arg1.s, "expunge")) {
                    c = getword(imapd_in, &arg1);
                    if (!imparse_issequence(arg1.s)) goto badsequence;
                    if (c == '\r') c = prot_getc(imapd_in);
                    if (c != '\n') goto extraargs;
                    cmd_expunge(tag.s, arg1.s);

                    snmp_increment(EXPUNGE_COUNT, 1);
                }
                else if (!strcmp(arg1.s, "xrunannotator")) {
                    goto xrunannotator;
                }
                else {
                    prot_printf(imapd_out, "%s BAD Unrecognized UID subcommand\r\n", tag.s);
                    eatline(imapd_in, c);
                }
            }
            else if (!strcmp(cmd.s, "Unsubscribe")) {
                if (c != ' ') goto missingargs;
                havenamespace = 0;
                c = getastring(imapd_in, imapd_out, &arg1);
                if (c == ' ') {
                    havenamespace = 1;
                    c = getastring(imapd_in, imapd_out, &arg2);
                }
                if (c == EOF) goto missingargs;
                if (c == '\r') c = prot_getc(imapd_in);
                if (c != '\n') goto extraargs;
                if (havenamespace) {
                    cmd_changesub(tag.s, arg1.s, arg2.s, 0);
                }
                else {
                    cmd_changesub(tag.s, (char *)0, arg1.s, 0);
                }

                snmp_increment(UNSUBSCRIBE_COUNT, 1);
            }
            else if (!strcmp(cmd.s, "Unselect")) {
                if (!imapd_index && !backend_current) goto nomailbox;
                if (c == '\r') c = prot_getc(imapd_in);
                if (c != '\n') goto extraargs;

                cmd_close(tag.s, cmd.s);

                snmp_increment(UNSELECT_COUNT, 1);
            }
            else if (!strcmp(cmd.s, "Undump")) {
                if(c != ' ') goto missingargs;
                c = getastring(imapd_in, imapd_out, &arg1);

                /* we want to get a list at this point */
                if(c != ' ') goto missingargs;

                cmd_undump(tag.s, arg1.s);
            /*  snmp_increment(UNDUMP_COUNT, 1);*/
            }
#ifdef HAVE_SSL
            else if (!strcmp(cmd.s, "Urlfetch")) {
                if (c != ' ') goto missingargs;

                cmd_urlfetch(tag.s);
            /*  snmp_increment(URLFETCH_COUNT, 1);*/
            }
#endif
            else goto badcmd;
            break;

        case 'X':
            if (!strcmp(cmd.s, "Xbackup")) {
                int havechannel = 0;

                if (!config_getswitch(IMAPOPT_XBACKUP_ENABLED))
                    goto badcmd;

                /* user */
                if (c != ' ') goto missingargs;
                c = getastring(imapd_in, imapd_out, &arg1);

                /* channel */
                if (c == ' ') {
                    havechannel = 1;
                    c = getword(imapd_in, &arg2);
                    if (c == EOF) goto missingargs;
                }

                if (c == '\r') c = prot_getc(imapd_in);
                if (c != '\n') goto extraargs;

                cmd_xbackup(tag.s, arg1.s, havechannel ? arg2.s : NULL);

            }
            else if (!strcmp(cmd.s, "Xconvfetch")) {
                cmd_xconvfetch(tag.s);

            }
            else if (!strcmp(cmd.s, "Xconvmultisort")) {
                if (c != ' ') goto missingargs;
                if (!imapd_index && !backend_current) goto nomailbox;
                cmd_xconvmultisort(tag.s);

            }
            else if (!strcmp(cmd.s, "Xconvsort")) {
                if (c != ' ') goto missingargs;
                if (!imapd_index && !backend_current) goto nomailbox;
                cmd_xconvsort(tag.s, 0);

            }
            else if (!strcmp(cmd.s, "Xconvupdates")) {
                if (c != ' ') goto missingargs;
                if (!imapd_index && !backend_current) goto nomailbox;
                cmd_xconvsort(tag.s, 1);

            }
            else if (!strcmp(cmd.s, "Xfer")) {
                int havepartition = 0;

                /* Mailbox */
                if(c != ' ') goto missingargs;
                c = getastring(imapd_in, imapd_out, &arg1);

                /* Dest Server */
                if(c != ' ') goto missingargs;
                c = getastring(imapd_in, imapd_out, &arg2);

                if(c == ' ') {
                    /* Dest Partition */
                    c = getastring(imapd_in, imapd_out, &arg3);
                    if (!imparse_isatom(arg3.s)) goto badpartition;
                    havepartition = 1;
                }

                if (c == '\r') c = prot_getc(imapd_in);
                if (c != '\n') goto extraargs;

                cmd_xfer(tag.s, arg1.s, arg2.s,
                         (havepartition ? arg3.s : NULL));
            /*  snmp_increment(XFER_COUNT, 1);*/
            }
            else if (!strcmp(cmd.s, "Xconvmeta")) {
                cmd_xconvmeta(tag.s);
            }
            else if (!strcmp(cmd.s, "Xlist")) {
                struct listargs listargs;

                if (c != ' ') goto missingargs;

                memset(&listargs, 0, sizeof(struct listargs));
                listargs.cmd = LIST_CMD_XLIST;
                listargs.ret = LIST_RET_CHILDREN | LIST_RET_SPECIALUSE;
                getlistargs(tag.s, &listargs);
                if (listargs.pat.count) cmd_list(tag.s, &listargs);

                snmp_increment(LIST_COUNT, 1);
            }
            else if (!strcmp(cmd.s, "Xmove")) {
                if (!imapd_index && !backend_current) goto nomailbox;
                usinguid = 0;
                if (c != ' ') goto missingargs;
                goto move;
            }
            else if (!strcmp(cmd.s, "Xrunannotator")) {
                if (!imapd_index && !backend_current) goto nomailbox;
                usinguid = 0;
                if (c != ' ') goto missingargs;
            xrunannotator:
                c = getword(imapd_in, &arg1);
                if (!arg1.len || !imparse_issequence(arg1.s)) goto badsequence;
                cmd_xrunannotator(tag.s, arg1.s, usinguid);
            }
            else if (!strcmp(cmd.s, "Xsnippets")) {
                if (c != ' ') goto missingargs;
                if (!imapd_index && !backend_current) goto nomailbox;
                cmd_xsnippets(tag.s);

            }
            else if (!strcmp(cmd.s, "Xstats")) {
                cmd_xstats(tag.s, c);
            }
            else if (!strcmp(cmd.s, "Xwarmup")) {
                /* XWARMUP doesn't need a mailbox to be selected */
                if (c != ' ') goto missingargs;
                cmd_xwarmup(tag.s);
            }
            else if (!strcmp(cmd.s, "Xkillmy")) {
                if (c != ' ') goto missingargs;
                c = getastring(imapd_in, imapd_out, &arg1);
                if (c == EOF) goto missingargs;
                if (c == '\r') c = prot_getc(imapd_in);
                if (c != '\n') goto extraargs;
                cmd_xkillmy(tag.s, arg1.s);
            }
            else if (!strcmp(cmd.s, "Xforever")) {
                if (c == '\r') c = prot_getc(imapd_in);
                if (c != '\n') goto extraargs;
                cmd_xforever(tag.s);
            }
            else if (!strcmp(cmd.s, "Xmeid")) {
                if (c != ' ') goto missingargs;
                c = getastring(imapd_in, imapd_out, &arg1);
                if (c == EOF) goto missingargs;
                if (c == '\r') c = prot_getc(imapd_in);
                if (c != '\n') goto extraargs;
                cmd_xmeid(tag.s, arg1.s);
            }

            else if (apns_enabled && !strcmp(cmd.s, "Xapplepushservice")) {
                if (c != ' ') goto missingargs;

                memset(&applepushserviceargs, 0, sizeof(struct applepushserviceargs));

                do {
                    c = getastring(imapd_in, imapd_out, &arg1);
                    if (c == EOF) goto aps_missingargs;

                    if (!strcmp(arg1.s, "mailboxes")) {
                        c = prot_getc(imapd_in);
                        if (c != '(')
                            goto aps_missingargs;

                        c = prot_getc(imapd_in);
                        if (c != ')') {
                            prot_ungetc(c, imapd_in);
                            do {
                                c = getastring(imapd_in, imapd_out, &arg2);
                                if (c == EOF) break;
                                strarray_push(&applepushserviceargs.mailboxes, arg2.s);
                            } while (c == ' ');
                        }

                        if (c != ')')
                            goto aps_missingargs;
                        c = prot_getc(imapd_in);
                    }

                    else {
                        c = getastring(imapd_in, imapd_out, &arg2);

                        if (!strcmp(arg1.s, "aps-version")) {
                            if (!imparse_isnumber(arg2.s)) goto aps_extraargs;
                            applepushserviceargs.aps_version = atoi(arg2.s);
                        }
                        else if (!strcmp(arg1.s, "aps-account-id"))
                            buf_copy(&applepushserviceargs.aps_account_id, &arg2);
                        else if (!strcmp(arg1.s, "aps-device-token"))
                            buf_copy(&applepushserviceargs.aps_device_token, &arg2);
                        else if (!strcmp(arg1.s, "aps-subtopic"))
                            buf_copy(&applepushserviceargs.aps_subtopic, &arg2);
                        else
                            goto aps_extraargs;
                    }
                } while (c == ' ');

                if (c == '\r') c = prot_getc(imapd_in);
                if (c != '\n') goto aps_extraargs;

                cmd_xapplepushservice(tag.s, &applepushserviceargs);
            }

            else goto badcmd;
            break;

        default:
        badcmd:
            prot_printf(imapd_out, "%s BAD Unrecognized command\r\n", tag.s);
            eatline(imapd_in, c);
        }

        /* End command timer - don't log "idle" commands */
        if (commandmintimer && strcmp("idle", cmdname)) {
            double cmdtime, nettime;
            const char *mboxname = index_mboxname(imapd_index);
            if (!mboxname) mboxname = "<none>";
            cmdtime_endtimer(&cmdtime, &nettime);
            if (cmdtime >= commandmintimerd) {
                syslog(LOG_NOTICE, "cmdtimer: '%s' '%s' '%s' '%f' '%f' '%f'",
                    imapd_userid ? imapd_userid : "<none>", cmdname, mboxname,
                    cmdtime, nettime, cmdtime + nettime);
            }
        }
        continue;

    nologin:
        prot_printf(imapd_out, "%s BAD Please login first\r\n", tag.s);
        eatline(imapd_in, c);
        continue;

    nomailbox:
        prot_printf(imapd_out,
                    "%s BAD Please select a mailbox first\r\n", tag.s);
        eatline(imapd_in, c);
        continue;

    aps_missingargs:
        buf_free(&applepushserviceargs.aps_account_id);
        buf_free(&applepushserviceargs.aps_device_token);
        buf_free(&applepushserviceargs.aps_subtopic);
        strarray_fini(&applepushserviceargs.mailboxes);

    missingargs:
        prot_printf(imapd_out,
                    "%s BAD Missing required argument to %s\r\n", tag.s, cmd.s);
        eatline(imapd_in, c);
        continue;

    aps_extraargs:
        buf_free(&applepushserviceargs.aps_account_id);
        buf_free(&applepushserviceargs.aps_device_token);
        buf_free(&applepushserviceargs.aps_subtopic);
        strarray_fini(&applepushserviceargs.mailboxes);

    extraargs:
        prot_printf(imapd_out,
                    "%s BAD Unexpected extra arguments to %s\r\n", tag.s, cmd.s);
        eatline(imapd_in, c);
        continue;

    badsequence:
        prot_printf(imapd_out,
                    "%s BAD Invalid sequence in %s\r\n", tag.s, cmd.s);
        eatline(imapd_in, c);
        continue;

    badpartition:
        prot_printf(imapd_out,
                    "%s BAD Invalid partition name in %s\r\n", tag.s, cmd.s);
        eatline(imapd_in, c);
        continue;
    }

done:
    cmd_syncrestart(NULL, &reserve_list, 0);
}
