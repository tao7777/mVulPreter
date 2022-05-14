OMX_ERRORTYPE  omx_video::free_buffer(OMX_IN OMX_HANDLETYPE         hComp,
        OMX_IN OMX_U32                 port,
        OMX_IN OMX_BUFFERHEADERTYPE* buffer)
{
 (void)hComp;
    OMX_ERRORTYPE eRet = OMX_ErrorNone;
 unsigned int nPortIndex;

    DEBUG_PRINT_LOW("In for encoder free_buffer");

 if (m_state == OMX_StateIdle &&
 (BITMASK_PRESENT(&m_flags ,OMX_COMPONENT_LOADING_PENDING))) {
        DEBUG_PRINT_LOW(" free buffer while Component in Loading pending");
 } else if ((m_sInPortDef.bEnabled == OMX_FALSE && port == PORT_INDEX_IN)||
 (m_sOutPortDef.bEnabled == OMX_FALSE && port == PORT_INDEX_OUT)) {
        DEBUG_PRINT_LOW("Free Buffer while port %u disabled", (unsigned int)port);
 } else if (m_state == OMX_StateExecuting || m_state == OMX_StatePause) {
        DEBUG_PRINT_ERROR("ERROR: Invalid state to free buffer,ports need to be disabled");
        post_event(OMX_EventError,
                OMX_ErrorPortUnpopulated,
                OMX_COMPONENT_GENERATE_EVENT);
 return eRet;
 } else {
        DEBUG_PRINT_ERROR("ERROR: Invalid state to free buffer,port lost Buffers");
        post_event(OMX_EventError,
                OMX_ErrorPortUnpopulated,
                OMX_COMPONENT_GENERATE_EVENT);
 }

 if (port == PORT_INDEX_IN) {
        nPortIndex = buffer - ((!meta_mode_enable)?m_inp_mem_ptr:meta_buffer_hdr);

 
         DEBUG_PRINT_LOW("free_buffer on i/p port - Port idx %u, actual cnt %u",
                 nPortIndex, (unsigned int)m_sInPortDef.nBufferCountActual);
        if (nPortIndex < m_sInPortDef.nBufferCountActual) {
             BITMASK_CLEAR(&m_inp_bm_count,nPortIndex);
             free_input_buffer (buffer);
            m_sInPortDef.bPopulated = OMX_FALSE;

 /*Free the Buffer Header*/
 if (release_input_done()
#ifdef _ANDROID_ICS_
 && !meta_mode_enable
#endif
 ) {
                input_use_buffer = false;
 if (m_inp_mem_ptr) {
                    DEBUG_PRINT_LOW("Freeing m_inp_mem_ptr");
                    free (m_inp_mem_ptr);
                    m_inp_mem_ptr = NULL;
 }
 if (m_pInput_pmem) {
                    DEBUG_PRINT_LOW("Freeing m_pInput_pmem");
                    free(m_pInput_pmem);
                    m_pInput_pmem = NULL;
 }
#ifdef USE_ION
 if (m_pInput_ion) {
                    DEBUG_PRINT_LOW("Freeing m_pInput_ion");
                    free(m_pInput_ion);
                    m_pInput_ion = NULL;
 }
#endif
 }
 } else {
            DEBUG_PRINT_ERROR("ERROR: free_buffer ,Port Index Invalid");
            eRet = OMX_ErrorBadPortIndex;
 }

 if (BITMASK_PRESENT((&m_flags),OMX_COMPONENT_INPUT_DISABLE_PENDING)
 && release_input_done()) {
            DEBUG_PRINT_LOW("MOVING TO DISABLED STATE");
            BITMASK_CLEAR((&m_flags),OMX_COMPONENT_INPUT_DISABLE_PENDING);
            post_event(OMX_CommandPortDisable,
                    PORT_INDEX_IN,
                    OMX_COMPONENT_GENERATE_EVENT);
 }
 } else if (port == PORT_INDEX_OUT) {
        nPortIndex = buffer - (OMX_BUFFERHEADERTYPE*)m_out_mem_ptr;

 
         DEBUG_PRINT_LOW("free_buffer on o/p port - Port idx %u, actual cnt %u",
                 nPortIndex, (unsigned int)m_sOutPortDef.nBufferCountActual);
        if (nPortIndex < m_sOutPortDef.nBufferCountActual) {
             BITMASK_CLEAR(&m_out_bm_count,nPortIndex);
             m_sOutPortDef.bPopulated = OMX_FALSE;
            free_output_buffer (buffer);

 if (release_output_done()) {
                output_use_buffer = false;
 if (m_out_mem_ptr) {
                    DEBUG_PRINT_LOW("Freeing m_out_mem_ptr");
                    free (m_out_mem_ptr);
                    m_out_mem_ptr = NULL;
 }
 if (m_pOutput_pmem) {
                    DEBUG_PRINT_LOW("Freeing m_pOutput_pmem");
                    free(m_pOutput_pmem);
                    m_pOutput_pmem = NULL;
 }
#ifdef USE_ION
 if (m_pOutput_ion) {
                    DEBUG_PRINT_LOW("Freeing m_pOutput_ion");
                    free(m_pOutput_ion);
                    m_pOutput_ion = NULL;
 }
#endif
 }
 } else {
            DEBUG_PRINT_ERROR("ERROR: free_buffer , Port Index Invalid");
            eRet = OMX_ErrorBadPortIndex;
 }
 if (BITMASK_PRESENT((&m_flags),OMX_COMPONENT_OUTPUT_DISABLE_PENDING)
 && release_output_done() ) {
            DEBUG_PRINT_LOW("FreeBuffer : If any Disable event pending,post it");

            DEBUG_PRINT_LOW("MOVING TO DISABLED STATE");
            BITMASK_CLEAR((&m_flags),OMX_COMPONENT_OUTPUT_DISABLE_PENDING);
            post_event(OMX_CommandPortDisable,
                    PORT_INDEX_OUT,
                    OMX_COMPONENT_GENERATE_EVENT);

 }
 } else {
        eRet = OMX_ErrorBadPortIndex;
 }
 if ((eRet == OMX_ErrorNone) &&
 (BITMASK_PRESENT(&m_flags ,OMX_COMPONENT_LOADING_PENDING))) {
 if (release_done()) {
 if (dev_stop() != 0) {
                DEBUG_PRINT_ERROR("ERROR: dev_stop() FAILED");
                eRet = OMX_ErrorHardware;
 }
            BITMASK_CLEAR((&m_flags),OMX_COMPONENT_LOADING_PENDING);
            post_event(OMX_CommandStateSet, OMX_StateLoaded,
                    OMX_COMPONENT_GENERATE_EVENT);
 } else {
            DEBUG_PRINT_HIGH("in free buffer, release not done, need to free more buffers input %" PRIx64" output %" PRIx64,
                    m_out_bm_count, m_inp_bm_count);
 }
 }

 return eRet;
}
