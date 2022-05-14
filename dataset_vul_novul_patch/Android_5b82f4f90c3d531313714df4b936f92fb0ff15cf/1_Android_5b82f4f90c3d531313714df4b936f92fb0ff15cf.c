OMX_ERRORTYPE  omx_vdec::empty_this_buffer(OMX_IN OMX_HANDLETYPE         hComp,
        OMX_IN OMX_BUFFERHEADERTYPE* buffer)
{

     OMX_ERRORTYPE ret1 = OMX_ErrorNone;
     unsigned int nBufferIndex = drv_ctx.ip_buf.actualcount;
 
    if (m_state == OMX_StateInvalid) {
         DEBUG_PRINT_ERROR("Empty this buffer in Invalid State");
         return OMX_ErrorInvalidState;
     }

 if (buffer == NULL) {
        DEBUG_PRINT_ERROR("ERROR:ETB Buffer is NULL");
 return OMX_ErrorBadParameter;
 }

 if (!m_inp_bEnabled) {
        DEBUG_PRINT_ERROR("ERROR:ETB incorrect state operation, input port is disabled.");
 return OMX_ErrorIncorrectStateOperation;
 }

 if (buffer->nInputPortIndex != OMX_CORE_INPUT_PORT_INDEX) {
        DEBUG_PRINT_ERROR("ERROR:ETB invalid port in header %u", (unsigned int)buffer->nInputPortIndex);
 return OMX_ErrorBadPortIndex;
 }

#ifdef _ANDROID_
 if (iDivXDrmDecrypt) {
        OMX_ERRORTYPE drmErr = iDivXDrmDecrypt->Decrypt(buffer);
 if (drmErr != OMX_ErrorNone) {
            DEBUG_PRINT_LOW("ERROR:iDivXDrmDecrypt->Decrypt %d", drmErr);
 }
 }
#endif //_ANDROID_
 if (perf_flag) {
 if (!latency) {
            dec_time.stop();
            latency = dec_time.processing_time_us();
            dec_time.start();
 }
 }

 if (arbitrary_bytes) {
        nBufferIndex = buffer - m_inp_heap_ptr;
 } else {
 if (input_use_buffer == true) {
            nBufferIndex = buffer - m_inp_heap_ptr;
            m_inp_mem_ptr[nBufferIndex].nFilledLen = m_inp_heap_ptr[nBufferIndex].nFilledLen;
            m_inp_mem_ptr[nBufferIndex].nTimeStamp = m_inp_heap_ptr[nBufferIndex].nTimeStamp;
            m_inp_mem_ptr[nBufferIndex].nFlags = m_inp_heap_ptr[nBufferIndex].nFlags;
            buffer = &m_inp_mem_ptr[nBufferIndex];
            DEBUG_PRINT_LOW("Non-Arbitrary mode - buffer address is: malloc %p, pmem%p in Index %d, buffer %p of size %u",
 &m_inp_heap_ptr[nBufferIndex], &m_inp_mem_ptr[nBufferIndex],nBufferIndex, buffer, (unsigned int)buffer->nFilledLen);
 } else {
            nBufferIndex = buffer - m_inp_mem_ptr;
 }
 }

 if (nBufferIndex > drv_ctx.ip_buf.actualcount ) {
        DEBUG_PRINT_ERROR("ERROR:ETB nBufferIndex is invalid");
 return OMX_ErrorBadParameter;
 }

 if (buffer->nFlags & OMX_BUFFERFLAG_CODECCONFIG) {
        codec_config_flag = true;
        DEBUG_PRINT_LOW("%s: codec_config buffer", __FUNCTION__);
 }

    DEBUG_PRINT_LOW("[ETB] BHdr(%p) pBuf(%p) nTS(%lld) nFL(%u)",
            buffer, buffer->pBuffer, buffer->nTimeStamp, (unsigned int)buffer->nFilledLen);
 if (arbitrary_bytes) {
        post_event ((unsigned long)hComp,(unsigned long)buffer,
                OMX_COMPONENT_GENERATE_ETB_ARBITRARY);
 } else {
        post_event ((unsigned long)hComp,(unsigned long)buffer,OMX_COMPONENT_GENERATE_ETB);
 }
    time_stamp_dts.insert_timestamp(buffer);
 return OMX_ErrorNone;
}
