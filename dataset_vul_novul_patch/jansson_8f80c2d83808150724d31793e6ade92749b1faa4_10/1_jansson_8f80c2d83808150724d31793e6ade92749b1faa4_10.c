int use_conf(char *test_path)
{
    int ret;
    size_t flags = 0;
    char filename[1024], errstr[1024];
    char *buffer;
    FILE *infile, *conffile;
    json_t *json;
    json_error_t error;

    sprintf(filename, "%s%cinput", test_path, dir_sep);
    if (!(infile = fopen(filename, "rb"))) {
        fprintf(stderr, "Could not open \"%s\"\n", filename);
        return 2;
    }

    sprintf(filename, "%s%cenv", test_path, dir_sep);
    conffile = fopen(filename, "rb");
    if (conffile) {
        read_conf(conffile);
        fclose(conffile);
    }

    if (conf.indent < 0 || conf.indent > 255) {
        fprintf(stderr, "invalid value for JSON_INDENT: %d\n", conf.indent);
        return 2;
    }

    if (conf.indent)
        flags |= JSON_INDENT(conf.indent);

    if (conf.compact)
        flags |= JSON_COMPACT;

    if (conf.ensure_ascii)
        flags |= JSON_ENSURE_ASCII;

    if (conf.preserve_order)
        flags |= JSON_PRESERVE_ORDER;

     if (conf.sort_keys)
         flags |= JSON_SORT_KEYS;
 
     if (conf.strip) {
         /* Load to memory, strip leading and trailing whitespace */
         buffer = loadfile(infile);
        json = json_loads(strip(buffer), 0, &error);
        free(buffer);
    }
    else
        json = json_loadf(infile, 0, &error);

    fclose(infile);

    if (!json) {
        sprintf(errstr, "%d %d %d\n%s\n",
                error.line, error.column, error.position,
                error.text);

        ret = cmpfile(errstr, test_path, "error");
        return ret;
    }

    buffer = json_dumps(json, flags);
    ret = cmpfile(buffer, test_path, "output");
    free(buffer);
    json_decref(json);

    return ret;
}
