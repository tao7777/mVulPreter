OMX_ERRORTYPE omx_video::free_input_buffer(OMX_BUFFERHEADERTYPE *bufferHdr)
{
 unsigned int index = 0;
    OMX_U8 *temp_buff ;

 if (bufferHdr == NULL || m_inp_mem_ptr == NULL) {
        DEBUG_PRINT_ERROR("ERROR: free_input: Invalid bufferHdr[%p] or m_inp_mem_ptr[%p]",
                bufferHdr, m_inp_mem_ptr);
 return OMX_ErrorBadParameter;
 }

    index = bufferHdr - ((!meta_mode_enable)?m_inp_mem_ptr:meta_buffer_hdr);
#ifdef _ANDROID_ICS_
 if (meta_mode_enable) {
 if (index < m_sInPortDef.nBufferCountActual) {
            memset(&meta_buffer_hdr[index], 0, sizeof(meta_buffer_hdr[index]));
            memset(&meta_buffers[index], 0, sizeof(meta_buffers[index]));
 }
 if (!mUseProxyColorFormat)
 return OMX_ErrorNone;
 else {
            c2d_conv.close();
            opaque_buffer_hdr[index] = NULL;
 }
 }
#endif
 if (index < m_sInPortDef.nBufferCountActual && !mUseProxyColorFormat &&
            dev_free_buf(&m_pInput_pmem[index],PORT_INDEX_IN) != true) {
        DEBUG_PRINT_LOW("ERROR: dev_free_buf() Failed for i/p buf");

     }
 
     if (index < m_sInPortDef.nBufferCountActual && m_pInput_pmem) {
         if (m_pInput_pmem[index].fd > 0 && input_use_buffer == false) {
             DEBUG_PRINT_LOW("FreeBuffer:: i/p AllocateBuffer case");
             if(!secure_session) {
            munmap (m_pInput_pmem[index].buffer,m_pInput_pmem[index].size);

             } else {
                 free(m_pInput_pmem[index].buffer);
             }
             close (m_pInput_pmem[index].fd);
 #ifdef USE_ION
             free_ion_memory(&m_pInput_ion[index]);
#endif
            m_pInput_pmem[index].fd = -1;
 } else if (m_pInput_pmem[index].fd > 0 && (input_use_buffer == true &&
                    m_use_input_pmem == OMX_FALSE)) {
            DEBUG_PRINT_LOW("FreeBuffer:: i/p Heap UseBuffer case");
 if (dev_free_buf(&m_pInput_pmem[index],PORT_INDEX_IN) != true) {

                 DEBUG_PRINT_ERROR("ERROR: dev_free_buf() Failed for i/p buf");
             }
             if(!secure_session) {
            munmap (m_pInput_pmem[index].buffer,m_pInput_pmem[index].size);
             }
             close (m_pInput_pmem[index].fd);
 #ifdef USE_ION
            free_ion_memory(&m_pInput_ion[index]);
#endif
            m_pInput_pmem[index].fd = -1;
 } else {
            DEBUG_PRINT_ERROR("FreeBuffer:: fd is invalid or i/p PMEM UseBuffer case");
 }
 }
 return OMX_ErrorNone;
}
