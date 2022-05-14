OMX_ERRORTYPE  omx_vdec::free_buffer(OMX_IN OMX_HANDLETYPE         hComp,
        OMX_IN OMX_U32                 port,
        OMX_IN OMX_BUFFERHEADERTYPE* buffer)
{
    OMX_ERRORTYPE eRet = OMX_ErrorNone;
 unsigned int nPortIndex;
 (void) hComp;
    DEBUG_PRINT_LOW("In for decoder free_buffer");

 if (m_state == OMX_StateIdle &&
 (BITMASK_PRESENT(&m_flags ,OMX_COMPONENT_LOADING_PENDING))) {
        DEBUG_PRINT_LOW(" free buffer while Component in Loading pending");
 } else if ((m_inp_bEnabled == OMX_FALSE && port == OMX_CORE_INPUT_PORT_INDEX)||
 (m_out_bEnabled == OMX_FALSE && port == OMX_CORE_OUTPUT_PORT_INDEX)) {
        DEBUG_PRINT_LOW("Free Buffer while port %u disabled", (unsigned int)port);
 } else if ((port == OMX_CORE_INPUT_PORT_INDEX &&
                BITMASK_PRESENT(&m_flags, OMX_COMPONENT_INPUT_ENABLE_PENDING)) ||
 (port == OMX_CORE_OUTPUT_PORT_INDEX &&
             BITMASK_PRESENT(&m_flags, OMX_COMPONENT_OUTPUT_ENABLE_PENDING))) {
        DEBUG_PRINT_LOW("Free Buffer while port %u enable pending", (unsigned int)port);
 } else if (m_state == OMX_StateExecuting || m_state == OMX_StatePause) {
        DEBUG_PRINT_ERROR("Invalid state to free buffer,ports need to be disabled");
        post_event(OMX_EventError,
                OMX_ErrorPortUnpopulated,
                OMX_COMPONENT_GENERATE_EVENT);

 return OMX_ErrorIncorrectStateOperation;
 } else if (m_state != OMX_StateInvalid) {
        DEBUG_PRINT_ERROR("Invalid state to free buffer,port lost Buffers");
        post_event(OMX_EventError,
                OMX_ErrorPortUnpopulated,
                OMX_COMPONENT_GENERATE_EVENT);
 }

 if (port == OMX_CORE_INPUT_PORT_INDEX) {
 /*Check if arbitrary bytes*/
 if (!arbitrary_bytes && !input_use_buffer)
            nPortIndex = buffer - m_inp_mem_ptr;
 else

             nPortIndex = buffer - m_inp_heap_ptr;
 
         DEBUG_PRINT_LOW("free_buffer on i/p port - Port idx %d", nPortIndex);
        if (nPortIndex < drv_ctx.ip_buf.actualcount) {
             BITMASK_CLEAR(&m_inp_bm_count,nPortIndex);
             BITMASK_CLEAR(&m_heap_inp_bm_count,nPortIndex);
 if (input_use_buffer == true) {

                DEBUG_PRINT_LOW("Free pmem Buffer index %d",nPortIndex);
 if (m_phdr_pmem_ptr)
                    free_input_buffer(m_phdr_pmem_ptr[nPortIndex]);
 } else {
 if (arbitrary_bytes) {
 if (m_phdr_pmem_ptr)
                        free_input_buffer(nPortIndex,m_phdr_pmem_ptr[nPortIndex]);
 else
                        free_input_buffer(nPortIndex,NULL);
 } else
                    free_input_buffer(buffer);
 }
            m_inp_bPopulated = OMX_FALSE;
 if(release_input_done())
                release_buffers(this, VDEC_BUFFER_TYPE_INPUT);
 /*Free the Buffer Header*/
 if (release_input_done()) {
                DEBUG_PRINT_HIGH("ALL input buffers are freed/released");
                free_input_buffer_header();
 }
 } else {
            DEBUG_PRINT_ERROR("Error: free_buffer ,Port Index Invalid");
            eRet = OMX_ErrorBadPortIndex;
 }

 if (BITMASK_PRESENT((&m_flags),OMX_COMPONENT_INPUT_DISABLE_PENDING)
 && release_input_done()) {
            DEBUG_PRINT_LOW("MOVING TO DISABLED STATE");
            BITMASK_CLEAR((&m_flags),OMX_COMPONENT_INPUT_DISABLE_PENDING);
            post_event(OMX_CommandPortDisable,
                    OMX_CORE_INPUT_PORT_INDEX,
                    OMX_COMPONENT_GENERATE_EVENT);
 }

     } else if (port == OMX_CORE_OUTPUT_PORT_INDEX) {
         nPortIndex = buffer - client_buffers.get_il_buf_hdr();
        if (nPortIndex < drv_ctx.op_buf.actualcount) {
             DEBUG_PRINT_LOW("free_buffer on o/p port - Port idx %d", nPortIndex);
             BITMASK_CLEAR(&m_out_bm_count,nPortIndex);
            m_out_bPopulated = OMX_FALSE;
            client_buffers.free_output_buffer (buffer);

 if(release_output_done()) {
                release_buffers(this, VDEC_BUFFER_TYPE_OUTPUT);
 }
 if (release_output_done()) {
                free_output_buffer_header();
 }
 } else {
            DEBUG_PRINT_ERROR("Error: free_buffer , Port Index Invalid");
            eRet = OMX_ErrorBadPortIndex;
 }
 if (BITMASK_PRESENT((&m_flags),OMX_COMPONENT_OUTPUT_DISABLE_PENDING)
 && release_output_done()) {
            DEBUG_PRINT_LOW("FreeBuffer : If any Disable event pending,post it");

            DEBUG_PRINT_LOW("MOVING TO DISABLED STATE");
            BITMASK_CLEAR((&m_flags),OMX_COMPONENT_OUTPUT_DISABLE_PENDING);
#ifdef _ANDROID_ICS_
 if (m_enable_android_native_buffers) {
                DEBUG_PRINT_LOW("FreeBuffer - outport disabled: reset native buffers");
                memset(&native_buffer, 0 ,(sizeof(struct nativebuffer) * MAX_NUM_INPUT_OUTPUT_BUFFERS));
 }
#endif

            post_event(OMX_CommandPortDisable,
                    OMX_CORE_OUTPUT_PORT_INDEX,
                    OMX_COMPONENT_GENERATE_EVENT);
 }
 } else {
        eRet = OMX_ErrorBadPortIndex;
 }
 if ((eRet == OMX_ErrorNone) &&
 (BITMASK_PRESENT(&m_flags ,OMX_COMPONENT_LOADING_PENDING))) {
 if (release_done()) {
            BITMASK_CLEAR((&m_flags),OMX_COMPONENT_LOADING_PENDING);
            post_event(OMX_CommandStateSet, OMX_StateLoaded,
                    OMX_COMPONENT_GENERATE_EVENT);
 }
 }
 return eRet;
}
