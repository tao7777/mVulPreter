OMX_ERRORTYPE  omx_video::empty_this_buffer(OMX_IN OMX_HANDLETYPE         hComp,
        OMX_IN OMX_BUFFERHEADERTYPE* buffer)
{
    OMX_ERRORTYPE ret1 = OMX_ErrorNone;

     unsigned int nBufferIndex ;
 
     DEBUG_PRINT_LOW("ETB: buffer = %p, buffer->pBuffer[%p]", buffer, buffer->pBuffer);
    if (m_state != OMX_StateExecuting &&
            m_state != OMX_StatePause &&
            m_state != OMX_StateIdle) {
         DEBUG_PRINT_ERROR("ERROR: Empty this buffer in Invalid State");
         return OMX_ErrorInvalidState;
     }

 if (buffer == NULL || (buffer->nSize != sizeof(OMX_BUFFERHEADERTYPE))) {
        DEBUG_PRINT_ERROR("ERROR: omx_video::etb--> buffer is null or buffer size is invalid");
 return OMX_ErrorBadParameter;
 }

 if (buffer->nVersion.nVersion != OMX_SPEC_VERSION) {
        DEBUG_PRINT_ERROR("ERROR: omx_video::etb--> OMX Version Invalid");
 return OMX_ErrorVersionMismatch;
 }

 if (buffer->nInputPortIndex != (OMX_U32)PORT_INDEX_IN) {
        DEBUG_PRINT_ERROR("ERROR: Bad port index to call empty_this_buffer");
 return OMX_ErrorBadPortIndex;
 }
 if (!m_sInPortDef.bEnabled) {
        DEBUG_PRINT_ERROR("ERROR: Cannot call empty_this_buffer while I/P port is disabled");
 return OMX_ErrorIncorrectStateOperation;
 }

    nBufferIndex = buffer - ((!meta_mode_enable)?m_inp_mem_ptr:meta_buffer_hdr);

 if (nBufferIndex > m_sInPortDef.nBufferCountActual ) {
        DEBUG_PRINT_ERROR("ERROR: ETB: Invalid buffer index[%d]", nBufferIndex);
 return OMX_ErrorBadParameter;
 }

    m_etb_count++;
    DEBUG_PRINT_LOW("DBG: i/p nTimestamp = %u", (unsigned)buffer->nTimeStamp);
    post_event ((unsigned long)hComp,(unsigned long)buffer,m_input_msg_id);
 return OMX_ErrorNone;
}
