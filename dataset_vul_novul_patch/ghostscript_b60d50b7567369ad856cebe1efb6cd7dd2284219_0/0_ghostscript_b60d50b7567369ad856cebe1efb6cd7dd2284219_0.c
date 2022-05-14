zstatus(i_ctx_t *i_ctx_p)
{
    os_ptr op = osp;

    switch (r_type(op)) {
        case t_file:
            {
                stream *s;

                make_bool(op, (file_is_valid(s, op) ? 1 : 0));
            }
            return 0;
        case t_string:
            {
                gs_parsed_file_name_t pname;
                struct stat fstat;
                int code = parse_file_name(op, &pname,
                                           i_ctx_p->LockFilePermissions, imemory);
                if (code < 0) {
                    if (code == gs_error_undefinedfilename) {
                        make_bool(op, 0);
                        code = 0;
                    }
                    return code;
                }
                code = gs_terminate_file_name(&pname, imemory, "status");
                if (code < 0)
                    return code;
                 code = gs_terminate_file_name(&pname, imemory, "status");
                 if (code < 0)
                     return code;
                if ((code = check_file_permissions(i_ctx_p, pname.fname, pname.len,
                                       "PermitFileReading")) >= 0) {
                    code = (*pname.iodev->procs.file_status)(pname.iodev,
                                                        pname.fname, &fstat);
                }
                 switch (code) {
                     case 0:
                         check_ostack(4);
                        make_int(op - 4, stat_blocks(&fstat));
                        make_int(op - 3, fstat.st_size);
                        /*
                         * We can't check the value simply by using ==,
                         * because signed/unsigned == does the wrong thing.
                         * Instead, since integer assignment only keeps the
                         * bottom bits, we convert the values to double
                         * and then test for equality.  This handles all
                         * cases of signed/unsigned or width mismatch.
                         */
                        if ((double)op[-4].value.intval !=
                              (double)stat_blocks(&fstat) ||
                            (double)op[-3].value.intval !=
                              (double)fstat.st_size
                            )
                            return_error(gs_error_limitcheck);
                        make_int(op - 2, fstat.st_mtime);
                        make_int(op - 1, fstat.st_ctime);
                        make_bool(op, 1);
                        break;
                    case gs_error_undefinedfilename:
                        make_bool(op, 0);
                        code = 0;
                }
                gs_free_file_name(&pname, "status");
                return code;
            }
        default:
            return_op_typecheck(op);
    }
}
