z_check_file_permissions(gs_memory_t *mem, const char *fname, const int len, const char *permission)
{
    i_ctx_t *i_ctx_p = get_minst_from_memory(mem)->i_ctx_p;
    gs_parsed_file_name_t pname;
    const char *permitgroup = permission[0] == 'r' ? "PermitFileReading" : "PermitFileWriting";
    int code = gs_parse_file_name(&pname, fname, len, imemory);
     if (code < 0)
         return code;
 
    if (pname.iodev && i_ctx_p->LockFilePermissions && strcmp(pname.iodev->dname, "%pipe%") == 0)
        return gs_error_invalidfileaccess;
    code = check_file_permissions(i_ctx_p, fname, len, permitgroup);
     return code;
 }
