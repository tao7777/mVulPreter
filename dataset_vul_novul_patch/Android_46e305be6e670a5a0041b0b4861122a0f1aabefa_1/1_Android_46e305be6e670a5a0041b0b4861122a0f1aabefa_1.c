bool omx_vdec::release_output_done(void)
{

     bool bRet = false;
     unsigned i=0,j=0;
 
    DEBUG_PRINT_LOW("Value of m_out_mem_ptr %p",m_inp_mem_ptr);
     if (m_out_mem_ptr) {
         for (; j < drv_ctx.op_buf.actualcount ; j++) {
             if (BITMASK_PRESENT(&m_out_bm_count,j)) {
 break;
 }
 }
 if (j == drv_ctx.op_buf.actualcount) {
            m_out_bm_count = 0;
            bRet = true;
 }
 } else {
        m_out_bm_count = 0;
        bRet = true;
 }
 return bRet;
}
