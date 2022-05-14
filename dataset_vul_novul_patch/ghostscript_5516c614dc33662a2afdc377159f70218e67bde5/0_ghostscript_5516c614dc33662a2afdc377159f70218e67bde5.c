zrestore(i_ctx_t *i_ctx_p)
restore_check_save(i_ctx_t *i_ctx_p, alloc_save_t **asave)
 {
     os_ptr op = osp;
    int code = restore_check_operand(op, asave, idmemory);
 
     if (code < 0)
         return code;
     if_debug2m('u', imemory, "[u]vmrestore 0x%lx, id = %lu\n",
               (ulong) alloc_save_client_data(*asave),
                (ulong) op->value.saveid);
     if (I_VALIDATE_BEFORE_RESTORE)
         ivalidate_clean_spaces(i_ctx_p);
        ivalidate_clean_spaces(i_ctx_p);
    /* Check the contents of the stacks. */
     {
         int code;
 
        if ((code = restore_check_stack(i_ctx_p, &o_stack, *asave, false)) < 0 ||
            (code = restore_check_stack(i_ctx_p, &e_stack, *asave, true)) < 0 ||
            (code = restore_check_stack(i_ctx_p, &d_stack, *asave, false)) < 0
             ) {
             osp++;
             return code;
         }
     }
    osp++;
    return 0;
}

/* the semantics of restore differ slightly between Level 1 and
   Level 2 and later - the latter includes restoring the device
   state (whilst Level 1 didn't have "page devices" as such).
   Hence we have two restore operators - one here (Level 1)
   and one in zdevice2.c (Level 2+). For that reason, the
   operand checking and guts of the restore operation are
   separated so both implementations can use them to best
   effect.
 */
int
dorestore(i_ctx_t *i_ctx_p, alloc_save_t *asave)
{
    os_ptr op = osp;
    bool last;
    vm_save_t *vmsave;
    int code;

    osp--;

     /* Reset l_new in all stack entries if the new save level is zero. */
     /* Also do some special fixing on the e-stack. */
     restore_fix_stack(i_ctx_p, &o_stack, asave, false);
    }
