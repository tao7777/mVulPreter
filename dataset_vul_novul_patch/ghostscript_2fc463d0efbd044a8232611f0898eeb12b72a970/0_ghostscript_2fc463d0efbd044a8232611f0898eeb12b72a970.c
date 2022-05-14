file_continue(i_ctx_t *i_ctx_p)
{
    os_ptr op = osp;
    es_ptr pscratch = esp - 2;
    file_enum *pfen = r_ptr(esp - 1, file_enum);
    int devlen = esp[-3].value.intval;
    gx_io_device *iodev = r_ptr(esp - 4, gx_io_device);
    uint len = r_size(pscratch);
    uint code;

    if (len < devlen)
        return_error(gs_error_rangecheck);     /* not even room for device len */

    do {
        memcpy((char *)pscratch->value.bytes, iodev->dname, devlen);
        code = iodev->procs.enumerate_next(pfen, (char *)pscratch->value.bytes + devlen,
                    len - devlen);
        if (code == ~(uint) 0) {    /* all done */
            esp -= 5;               /* pop proc, pfen, devlen, iodev , mark */
            return o_pop_estack;
         } else if (code > len)      /* overran string */
             return_error(gs_error_rangecheck);
         else if (iodev != iodev_default(imemory)
              || (check_file_permissions(i_ctx_p, (char *)pscratch->value.bytes, code + devlen, iodev, "PermitFileReading")) == 0) {
             push(1);
             ref_assign(op, pscratch);
             r_set_size(op, code + devlen);
            push_op_estack(file_continue);  /* come again */
            *++esp = pscratch[2];   /* proc */
            return o_push_estack;
        }
    } while(1);
}
