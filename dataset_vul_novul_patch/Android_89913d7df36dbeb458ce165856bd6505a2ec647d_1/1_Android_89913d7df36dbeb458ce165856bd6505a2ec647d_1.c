OMX_ERRORTYPE  omx_venc::component_deinit(OMX_IN OMX_HANDLETYPE hComp)
{
 (void) hComp;
    OMX_U32 i = 0;
    DEBUG_PRINT_HIGH("omx_venc(): Inside component_deinit()");
 if (OMX_StateLoaded != m_state) {
        DEBUG_PRINT_ERROR("WARNING:Rxd DeInit,OMX not in LOADED state %d",\
                m_state);
 }

     if (m_out_mem_ptr) {
         DEBUG_PRINT_LOW("Freeing the Output Memory");
         for (i=0; i< m_sOutPortDef.nBufferCountActual; i++ ) {
            free_output_buffer (&m_out_mem_ptr[i]);
         }
         free(m_out_mem_ptr);
         m_out_mem_ptr = NULL;
 }

 /*Check if the input buffers have to be cleaned up*/
 if (m_inp_mem_ptr
#ifdef _ANDROID_ICS_
 && !meta_mode_enable
#endif

        ) {
         DEBUG_PRINT_LOW("Freeing the Input Memory");
         for (i=0; i<m_sInPortDef.nBufferCountActual; i++ ) {
            free_input_buffer (&m_inp_mem_ptr[i]);
         }
 
 
        free(m_inp_mem_ptr);
        m_inp_mem_ptr = NULL;
 }

    m_ftb_q.m_size=0;
    m_cmd_q.m_size=0;
    m_etb_q.m_size=0;
    m_ftb_q.m_read = m_ftb_q.m_write =0;
    m_cmd_q.m_read = m_cmd_q.m_write =0;
    m_etb_q.m_read = m_etb_q.m_write =0;

#ifdef _ANDROID_
    DEBUG_PRINT_HIGH("Calling m_heap_ptr.clear()");
    m_heap_ptr.clear();
#endif // _ANDROID_
    DEBUG_PRINT_HIGH("Calling venc_close()");
 if (handle) {
        handle->venc_close();
        DEBUG_PRINT_HIGH("Deleting HANDLE[%p]", handle);
 delete (handle);
        handle = NULL;
 }
    DEBUG_PRINT_INFO("Component Deinit");
 return OMX_ErrorNone;
}
