 static void cliRefreshPrompt(void) {
    int len;
     if (config.eval_ldb) return;
    if (config.hostsocket != NULL)
        len = snprintf(config.prompt,sizeof(config.prompt),"redis %s",
                       config.hostsocket);
    else
        len = anetFormatAddr(config.prompt, sizeof(config.prompt),
                           config.hostip, config.hostport);
     /* Add [dbnum] if needed */
     if (config.dbnum != 0)
        len += snprintf(config.prompt+len,sizeof(config.prompt)-len,"[%d]",
            config.dbnum);
    snprintf(config.prompt+len,sizeof(config.prompt)-len,"> ");
 }
