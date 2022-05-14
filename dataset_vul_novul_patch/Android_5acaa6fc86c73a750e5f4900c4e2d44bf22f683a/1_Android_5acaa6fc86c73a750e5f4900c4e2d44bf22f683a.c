 WORD32 ih264d_create(iv_obj_t *dec_hdl, void *pv_api_ip, void *pv_api_op)
 {
     ih264d_create_op_t *ps_create_op;
 
     WORD32 ret;
 
     ps_create_op = (ih264d_create_op_t *)pv_api_op;
 
     ps_create_op->s_ivd_create_op_t.u4_error_code = 0;
     ret = ih264d_allocate_static_bufs(&dec_hdl, pv_api_ip, pv_api_op);
 
     /* If allocation of some buffer fails, then free buffers allocated till then */
    if((IV_FAIL == ret) && (NULL != dec_hdl))
     {
        ih264d_free_static_bufs(dec_hdl);
         ps_create_op->s_ivd_create_op_t.u4_error_code = IVD_MEM_ALLOC_FAILED;
         ps_create_op->s_ivd_create_op_t.u4_error_code = 1 << IVD_FATALERROR;
 
 return IV_FAIL;
 }

 return IV_SUCCESS;
}
