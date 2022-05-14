int use_env()
{
    int indent;
    size_t flags = 0;
    json_t *json;
    json_error_t error;

    #ifdef _WIN32
    /* On Windows, set stdout and stderr to binary mode to avoid
       outputting DOS line terminators */
    _setmode(_fileno(stdout), _O_BINARY);
    _setmode(_fileno(stderr), _O_BINARY);
    #endif

    indent = getenv_int("JSON_INDENT");
    if(indent < 0 || indent > 255) {
        fprintf(stderr, "invalid value for JSON_INDENT: %d\n", indent);
        return 2;
    }

    if(indent > 0)
        flags |= JSON_INDENT(indent);

    if(getenv_int("JSON_COMPACT") > 0)
        flags |= JSON_COMPACT;

    if(getenv_int("JSON_ENSURE_ASCII"))
        flags |= JSON_ENSURE_ASCII;

    if(getenv_int("JSON_PRESERVE_ORDER"))
         flags |= JSON_PRESERVE_ORDER;
 
     if(getenv_int("JSON_SORT_KEYS"))
        flags |= JSON_SORT_KEYS;

    if(getenv("HASHSEED"))
        json_object_seed(getenv_int("HASHSEED"));
 
     if(getenv_int("STRIP")) {
         /* Load to memory, strip leading and trailing whitespace */
        size_t size = 0, used = 0;
        char *buffer = NULL;

        while(1) {
            size_t count;

            size = (size == 0 ? 128 : size * 2);
            buffer = realloc(buffer, size);
            if(!buffer) {
                fprintf(stderr, "Unable to allocate %d bytes\n", (int)size);
                return 1;
            }

            count = fread(buffer + used, 1, size - used, stdin);
            if(count < size - used) {
                buffer[used + count] = '\0';
                break;
            }
            used += count;
        }

        json = json_loads(strip(buffer), 0, &error);
        free(buffer);
    }
    else
        json = json_loadf(stdin, 0, &error);

    if(!json) {
        fprintf(stderr, "%d %d %d\n%s\n",
            error.line, error.column,
            error.position, error.text);
        return 1;
    }

    json_dumpf(json, stdout, flags);
    json_decref(json);

    return 0;
}
