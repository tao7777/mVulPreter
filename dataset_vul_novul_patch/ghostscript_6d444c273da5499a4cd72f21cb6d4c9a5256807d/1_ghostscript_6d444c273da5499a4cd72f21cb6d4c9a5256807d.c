gsicc_open_search(const char* pname, int namelen, gs_memory_t *mem_gc,
                  const char* dirname, int dirlen, stream**strp)
{
    char *buffer;
    stream* str;

    /* Check if we need to prepend the file name  */
    if ( dirname != NULL) {
        /* If this fails, we will still try the file by itself and with
           %rom% since someone may have left a space some of the spaces
           as our defaults, even if they defined the directory to use.
           This will occur only after searching the defined directory.
           A warning is noted.  */
        buffer = (char *) gs_alloc_bytes(mem_gc, namelen + dirlen + 1,
                                     "gsicc_open_search");
        if (buffer == NULL)
            return_error(gs_error_VMerror);
        strcpy(buffer, dirname);
        strcat(buffer, pname);
        /* Just to make sure we were null terminated */
        buffer[namelen + dirlen] = '\0';
        str = sfopen(buffer, "r", mem_gc);
        gs_free_object(mem_gc, buffer, "gsicc_open_search");
        if (str != NULL) {
            *strp = str;
	    return 0;
        }
     }
 
     /* First just try it like it is */
    str = sfopen(pname, "r", mem_gc);
    if (str != NULL) {
        *strp = str;
        return 0;
     }
 
     /* If that fails, try %rom% */ /* FIXME: Not sure this is needed or correct */
                        strlen(DEFAULT_DIR_ICC),"gsicc_open_search");
    if (buffer == NULL)
        return_error(gs_error_VMerror);
    strcpy(buffer, DEFAULT_DIR_ICC);
    strcat(buffer, pname);
    /* Just to make sure we were null terminated */
    buffer[namelen + strlen(DEFAULT_DIR_ICC)] = '\0';
    str = sfopen(buffer, "r", mem_gc);
    gs_free_object(mem_gc, buffer, "gsicc_open_search");
    if (str == NULL) {
        gs_warn1("Could not find %s ",pname);
    }
    *strp = str;
    return 0;
}
