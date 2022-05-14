WORD32 ihevcd_create(iv_obj_t *ps_codec_obj,

                            void *pv_api_ip,
                            void *pv_api_op)
 {
     ihevcd_cxa_create_op_t *ps_create_op;
 
     WORD32 ret;
     codec_t *ps_codec;
     ps_create_op = (ihevcd_cxa_create_op_t *)pv_api_op;
 
     ps_create_op->s_ivd_create_op_t.u4_error_code = 0;
     ret = ihevcd_allocate_static_bufs(&ps_codec_obj, pv_api_ip, pv_api_op);
 
     /* If allocation of some buffer fails, then free buffers allocated till then */
    if((IV_FAIL == ret) && (NULL != ps_codec_obj))
     {
        ihevcd_free_static_bufs(ps_codec_obj);
         ps_create_op->s_ivd_create_op_t.u4_error_code = IVD_MEM_ALLOC_FAILED;
         ps_create_op->s_ivd_create_op_t.u4_error_code = 1 << IVD_FATALERROR;
 
 return IV_FAIL;
 }
    ps_codec = (codec_t *)ps_codec_obj->pv_codec_handle;
    ret = ihevcd_init(ps_codec);

    TRACE_INIT(NULL);
    STATS_INIT();

 return ret;
}
