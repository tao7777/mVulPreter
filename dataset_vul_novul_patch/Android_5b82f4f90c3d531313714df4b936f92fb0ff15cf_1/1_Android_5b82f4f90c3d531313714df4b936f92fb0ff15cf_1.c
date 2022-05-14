 OMX_ERRORTYPE  omx_vdec::fill_this_buffer(OMX_IN OMX_HANDLETYPE  hComp,
         OMX_IN OMX_BUFFERHEADERTYPE* buffer)
 {
     unsigned nPortIndex = 0;
     if (dynamic_buf_mode) {
         private_handle_t *handle = NULL;
 struct VideoDecoderOutputMetaData *meta;
 unsigned int nPortIndex = 0;

 if (!buffer || !buffer->pBuffer) {
            DEBUG_PRINT_ERROR("%s: invalid params: %p", __FUNCTION__, buffer);
 return OMX_ErrorBadParameter;
 }

        meta = (struct VideoDecoderOutputMetaData *)buffer->pBuffer;
        handle = (private_handle_t *)meta->pHandle;
        DEBUG_PRINT_LOW("FTB: metabuf: %p buftype: %d bufhndl: %p ", meta, meta->eType, meta->pHandle);

 if (!handle) {
            DEBUG_PRINT_ERROR("FTB: Error: IL client passed an invalid buf handle - %p", handle);
 return OMX_ErrorBadParameter;
 }

        nPortIndex = buffer-((OMX_BUFFERHEADERTYPE *)client_buffers.get_il_buf_hdr());
        drv_ctx.ptr_outputbuffer[nPortIndex].pmem_fd = handle->fd;
        drv_ctx.ptr_outputbuffer[nPortIndex].bufferaddr = (OMX_U8*) buffer;

        native_buffer[nPortIndex].privatehandle = handle;
        native_buffer[nPortIndex].nativehandle = handle;

        buffer->nFilledLen = 0;

         buffer->nAllocLen = handle->size;
     }
 
    if (m_state == OMX_StateInvalid) {
        DEBUG_PRINT_ERROR("FTB in Invalid State");
        return OMX_ErrorInvalidState;
    }
    if (!m_out_bEnabled) {
        DEBUG_PRINT_ERROR("ERROR:FTB incorrect state operation, output port is disabled.");
        return OMX_ErrorIncorrectStateOperation;
    }
     nPortIndex = buffer - client_buffers.get_il_buf_hdr();
     if (buffer == NULL ||
             (nPortIndex >= drv_ctx.op_buf.actualcount)) {
        DEBUG_PRINT_ERROR("FTB: ERROR: invalid buffer index, nPortIndex %u bufCount %u",
            nPortIndex, drv_ctx.op_buf.actualcount);
 return OMX_ErrorBadParameter;
 }

 if (buffer->nOutputPortIndex != OMX_CORE_OUTPUT_PORT_INDEX) {
        DEBUG_PRINT_ERROR("ERROR:FTB invalid port in header %u", (unsigned int)buffer->nOutputPortIndex);
 return OMX_ErrorBadPortIndex;
 }

    DEBUG_PRINT_LOW("[FTB] bufhdr = %p, bufhdr->pBuffer = %p", buffer, buffer->pBuffer);
    post_event((unsigned long) hComp, (unsigned long)buffer, m_fill_output_msg);
 return OMX_ErrorNone;
}
