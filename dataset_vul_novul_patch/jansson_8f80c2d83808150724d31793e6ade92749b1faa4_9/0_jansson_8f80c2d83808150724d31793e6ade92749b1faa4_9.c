static void read_conf(FILE *conffile)
{
    char *buffer, *line, *val;

    buffer = loadfile(conffile);
    for (line = strtok(buffer, "\r\n"); line; line = strtok(NULL, "\r\n")) {
        if (!strncmp(line, "export ", 7))
            continue;
        val = strchr(line, '=');
        if (!val) {
            printf("invalid configuration line\n");
            break;
        }
        *val++ = '\0';

        if (!strcmp(line, "JSON_INDENT"))
            conf.indent = atoi(val);
        if (!strcmp(line, "JSON_COMPACT"))
            conf.compact = atoi(val);
        if (!strcmp(line, "JSON_ENSURE_ASCII"))
            conf.ensure_ascii = atoi(val);
        if (!strcmp(line, "JSON_PRESERVE_ORDER"))
            conf.preserve_order = atoi(val);
        if (!strcmp(line, "JSON_SORT_KEYS"))
             conf.sort_keys = atoi(val);
         if (!strcmp(line, "STRIP"))
             conf.strip = atoi(val);
        if (!strcmp(line, "HASHSEED")) {
            conf.have_hashseed = 1;
            conf.hashseed = atoi(val);
        } else {
            conf.have_hashseed = 0;
        }
     }
 
     free(buffer);
}
