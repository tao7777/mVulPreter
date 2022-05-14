static int mboxlist_do_find(struct find_rock *rock, const strarray_t *patterns)
{
    const char *userid = rock->userid;
    int isadmin = rock->isadmin;

    int crossdomains = config_getswitch(IMAPOPT_CROSSDOMAINS);
    char inbox[MAX_MAILBOX_BUFFER];
    size_t inboxlen = 0;
    size_t prefixlen, len;
    size_t domainlen = 0;
    size_t userlen = userid ? strlen(userid) : 0;
    char domainpat[MAX_MAILBOX_BUFFER]; /* do intra-domain fetches only */
    char commonpat[MAX_MAILBOX_BUFFER];
    int r = 0;
    int i;
    const char *p;

    if (patterns->count < 1) return 0; /* nothing to do */

    for (i = 0; i < patterns->count; i++) {
        glob *g = glob_init(strarray_nth(patterns, i), rock->namespace->hier_sep);
        ptrarray_append(&rock->globs, g);
    }

    if (config_virtdomains && userid && (p = strchr(userid, '@'))) {
        userlen = p - userid;
        domainlen = strlen(p); /* includes separator */
        snprintf(domainpat, sizeof(domainpat), "%s!", p+1);
    }
    else
        domainpat[0] = '\0';

    /* calculate the inbox (with trailing .INBOX. for later use) */
    if (userid && (!(p = strchr(userid, rock->namespace->hier_sep)) ||
        ((p - userid) > (int)userlen)) &&
        strlen(userid)+7 < MAX_MAILBOX_BUFFER) {
        char *t, *tmpuser = NULL;
        const char *inboxuser;

        if (domainlen)
            snprintf(inbox, sizeof(inbox), "%s!", userid+userlen+1);
        if (rock->namespace->hier_sep == '/' && (p = strchr(userid, '.'))) {
            tmpuser = xmalloc(userlen);
            memcpy(tmpuser, userid, userlen);
            t = tmpuser + (p - userid);
            while(t < (tmpuser + userlen)) {
                if (*t == '.')
                    *t = '^';
                t++;
            }
            inboxuser = tmpuser;
        } else
            inboxuser = userid;
        snprintf(inbox+domainlen, sizeof(inbox)-domainlen,
                 "user.%.*s.INBOX.", (int)userlen, inboxuser);
        free(tmpuser);
        inboxlen = strlen(inbox) - 7;
    }
    else {
        userid = 0;
    }

    /* Find the common search prefix of all patterns */
    const char *firstpat = strarray_nth(patterns, 0);
    for (prefixlen = 0; firstpat[prefixlen]; prefixlen++) {
        if (prefixlen >= MAX_MAILBOX_NAME) {
            r = IMAP_MAILBOX_BADNAME;
            goto done;
        }
        char c = firstpat[prefixlen];
        for (i = 1; i < patterns->count; i++) {
            const char *pat = strarray_nth(patterns, i);
            if (pat[prefixlen] != c) break;
        }
        if (i < patterns->count) break;
        if (c == '*' || c == '%' || c == '?') break;
        commonpat[prefixlen] = c;
    }
    commonpat[prefixlen] = '\0';

    if (patterns->count == 1) {
        /* Skip pattern which matches shared namespace prefix */
        if (!strcmp(firstpat+prefixlen, "%"))
            rock->singlepercent = 2;
        /* output prefix regardless */
        if (!strcmp(firstpat+prefixlen, "*%"))
            rock->singlepercent = 1;
    }

    /*
     * Personal (INBOX) namespace (only if not admin)
     */
    if (userid && !isadmin) {
        /* first the INBOX */
        rock->mb_category = MBNAME_INBOX;
        r = cyrusdb_forone(rock->db, inbox, inboxlen, &find_p, &find_cb, rock, NULL);
        if (r == CYRUSDB_DONE) r = 0;
        if (r) goto done;

        if (rock->namespace->isalt) {
            /* do exact INBOX subs before resetting the namebuffer */
            rock->mb_category = MBNAME_INBOXSUB;
            r = cyrusdb_foreach(rock->db, inbox, inboxlen+7, &find_p, &find_cb, rock, NULL);
            if (r == CYRUSDB_DONE) r = 0;
            if (r) goto done;

            /* reset the the namebuffer */
            r = (*rock->proc)(NULL, rock->procrock);
            if (r) goto done;
        }

        /* iterate through all the mailboxes under the user's inbox */
        rock->mb_category = MBNAME_OWNER;
        r = cyrusdb_foreach(rock->db, inbox, inboxlen+1, &find_p, &find_cb, rock, NULL);
        if (r == CYRUSDB_DONE) r = 0;
        if (r) goto done;

        /* "Alt Prefix" folders */
        if (rock->namespace->isalt) {
            /* reset the the namebuffer */
            r = (*rock->proc)(NULL, rock->procrock);
            if (r) goto done;

            rock->mb_category = MBNAME_ALTINBOX;

            /* special case user.foo.INBOX.  If we're singlepercent == 2, this could
             return DONE, in which case we don't need to foreach the rest of the
             altprefix space */
            r = cyrusdb_forone(rock->db, inbox, inboxlen+6, &find_p, &find_cb, rock, NULL);
            if (r == CYRUSDB_DONE) goto skipalt;
            if (r) goto done;

            /* special case any other altprefix stuff */
            rock->mb_category = MBNAME_ALTPREFIX;
            r = cyrusdb_foreach(rock->db, inbox, inboxlen+1, &find_p, &find_cb, rock, NULL);
        skipalt: /* we got a done, so skip out of the foreach early */
            if (r == CYRUSDB_DONE) r = 0;
            if (r) goto done;
        }
    }

    /*
     * Other Users namespace
     *
     * If "Other Users*" can match pattern, search for those mailboxes next
     */
    if (isadmin || rock->namespace->accessible[NAMESPACE_USER]) {
        len = strlen(rock->namespace->prefix[NAMESPACE_USER]);
         if (len) len--; // trailing separator
 
         if (!strncmp(rock->namespace->prefix[NAMESPACE_USER], commonpat, MIN(len, prefixlen))) {
            if (prefixlen <= len) {
                 /* we match all users */
                 strlcpy(domainpat+domainlen, "user.", sizeof(domainpat)-domainlen);
             }
            else {
                /* just those in this prefix */
                strlcpy(domainpat+domainlen, "user.", sizeof(domainpat)-domainlen);
                strlcpy(domainpat+domainlen+5, commonpat+len+1, sizeof(domainpat)-domainlen-5);
            }

            rock->mb_category = MBNAME_OTHERUSER;

            /* because of how domains work, with crossdomains or admin you can't prefix at all :( */
            size_t thislen = (isadmin || crossdomains) ? 0 : strlen(domainpat);

            /* reset the the namebuffer */
            r = (*rock->proc)(NULL, rock->procrock);
            if (r) goto done;

            r = mboxlist_find_category(rock, domainpat, thislen);
            if (r) goto done;
        }
    }

    /*
     * Shared namespace
     *
     * search for all remaining mailboxes.
     * just bother looking at the ones that have the same pattern prefix.
     */
    if (isadmin || rock->namespace->accessible[NAMESPACE_SHARED]) {
        len = strlen(rock->namespace->prefix[NAMESPACE_SHARED]);
        if (len) len--; // trailing separator

        if (!strncmp(rock->namespace->prefix[NAMESPACE_SHARED], commonpat, MIN(len, prefixlen))) {
            rock->mb_category = MBNAME_SHARED;

            /* reset the the namebuffer */
            r = (*rock->proc)(NULL, rock->procrock);
            if (r) goto done;

            /* iterate through all the non-user folders on the server */
            r = mboxlist_find_category(rock, domainpat, domainlen);
            if (r) goto done;
        }
    }

    /* finish with a reset call always */
    r = (*rock->proc)(NULL, rock->procrock);

 done:
    for (i = 0; i < rock->globs.count; i++) {
        glob *g = ptrarray_nth(&rock->globs, i);
        glob_free(&g);
    }
    ptrarray_fini(&rock->globs);

    return r;
}
