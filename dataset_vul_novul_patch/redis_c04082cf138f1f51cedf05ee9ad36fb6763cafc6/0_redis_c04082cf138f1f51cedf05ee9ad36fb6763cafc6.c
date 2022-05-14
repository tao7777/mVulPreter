void xgroupCommand(client *c) {
    const char *help[] = {
"CREATE      <key> <groupname> <id or $>  -- Create a new consumer group.",
"SETID       <key> <groupname> <id or $>  -- Set the current group ID.",
"DELGROUP    <key> <groupname>            -- Remove the specified group.",
"DELCONSUMER <key> <groupname> <consumer> -- Remove the specified conusmer.",
"HELP                                     -- Prints this help.",
NULL
    };
    stream *s = NULL;
    sds grpname = NULL;
    streamCG *cg = NULL;
    char *opt = c->argv[1]->ptr; /* Subcommand name. */

     /* Lookup the key now, this is common for all the subcommands but HELP. */
     if (c->argc >= 4) {
         robj *o = lookupKeyWriteOrReply(c,c->argv[2],shared.nokeyerr);
        if (o == NULL || checkType(c,o,OBJ_STREAM)) return;
         s = o->ptr;
         grpname = c->argv[3]->ptr;
 
        /* Certain subcommands require the group to exist. */
        if ((cg = streamLookupCG(s,grpname)) == NULL &&
            (!strcasecmp(opt,"SETID") ||
             !strcasecmp(opt,"DELCONSUMER")))
        {
            addReplyErrorFormat(c, "-NOGROUP No such consumer group '%s' "
                                   "for key name '%s'",
                                   (char*)grpname, (char*)c->argv[2]->ptr);
            return;
        }
    }

    /* Dispatch the different subcommands. */
    if (!strcasecmp(opt,"CREATE") && c->argc == 5) {
        streamID id;
        if (!strcmp(c->argv[4]->ptr,"$")) {
            id = s->last_id;
        } else if (streamParseIDOrReply(c,c->argv[4],&id,0) != C_OK) {
            return;
        }
        streamCG *cg = streamCreateCG(s,grpname,sdslen(grpname),&id);
        if (cg) {
            addReply(c,shared.ok);
            server.dirty++;
        } else {
            addReplySds(c,
                sdsnew("-BUSYGROUP Consumer Group name already exists\r\n"));
        }
    } else if (!strcasecmp(opt,"SETID") && c->argc == 5) {
        streamID id;
        if (!strcmp(c->argv[4]->ptr,"$")) {
            id = s->last_id;
        } else if (streamParseIDOrReply(c,c->argv[4],&id,0) != C_OK) {
            return;
        }
        cg->last_id = id;
        addReply(c,shared.ok);
    } else if (!strcasecmp(opt,"DESTROY") && c->argc == 4) {
        if (cg) {
            raxRemove(s->cgroups,(unsigned char*)grpname,sdslen(grpname),NULL);
            streamFreeCG(cg);
            addReply(c,shared.cone);
        } else {
            addReply(c,shared.czero);
        }
    } else if (!strcasecmp(opt,"DELCONSUMER") && c->argc == 5) {
        /* Delete the consumer and returns the number of pending messages
         * that were yet associated with such a consumer. */
        long long pending = streamDelConsumer(cg,c->argv[4]->ptr);
        addReplyLongLong(c,pending);
        server.dirty++;
    } else if (!strcasecmp(opt,"HELP")) {
        addReplyHelp(c, help);
    } else {
        addReply(c,shared.syntaxerr);
    }
}
