OMX_ERRORTYPE  omx_video::empty_this_buffer_proxy(OMX_IN OMX_HANDLETYPE  hComp,
        OMX_IN OMX_BUFFERHEADERTYPE* buffer)
{
 (void)hComp;
    OMX_U8 *pmem_data_buf = NULL;
 int push_cnt = 0;
 unsigned nBufIndex = 0;
    OMX_ERRORTYPE ret = OMX_ErrorNone;
    encoder_media_buffer_type *media_buffer = NULL;

#ifdef _MSM8974_
 int fd = 0;
#endif
    DEBUG_PRINT_LOW("ETBProxy: buffer->pBuffer[%p]", buffer->pBuffer);
 if (buffer == NULL) {
        DEBUG_PRINT_ERROR("ERROR: ETBProxy: Invalid buffer[%p]", buffer);
 return OMX_ErrorBadParameter;
 }

 if (meta_mode_enable && !mUsesColorConversion) {
 bool met_error = false;
        nBufIndex = buffer - meta_buffer_hdr;
 if (nBufIndex >= m_sInPortDef.nBufferCountActual) {
            DEBUG_PRINT_ERROR("ERROR: ETBProxy: Invalid meta-bufIndex = %u", nBufIndex);
 return OMX_ErrorBadParameter;
 }
        media_buffer = (encoder_media_buffer_type *)meta_buffer_hdr[nBufIndex].pBuffer;
 if (media_buffer) {
 if (media_buffer->buffer_type != kMetadataBufferTypeCameraSource &&
                    media_buffer->buffer_type != kMetadataBufferTypeGrallocSource) {
                met_error = true;
 } else {
 if (media_buffer->buffer_type == kMetadataBufferTypeCameraSource) {
 if (media_buffer->meta_handle == NULL)
                        met_error = true;
 else if ((media_buffer->meta_handle->numFds != 1 &&
                                media_buffer->meta_handle->numInts != 2))
                        met_error = true;
 }
 }
 } else
            met_error = true;
 if (met_error) {
            DEBUG_PRINT_ERROR("ERROR: Unkown source/metahandle in ETB call");
            post_event ((unsigned long)buffer,0,OMX_COMPONENT_GENERATE_EBD);
 return OMX_ErrorBadParameter;
 }
 } else {
        nBufIndex = buffer - ((OMX_BUFFERHEADERTYPE *)m_inp_mem_ptr);
 if (nBufIndex >= m_sInPortDef.nBufferCountActual) {
            DEBUG_PRINT_ERROR("ERROR: ETBProxy: Invalid bufIndex = %u", nBufIndex);
 return OMX_ErrorBadParameter;
 }
 }

    pending_input_buffers++;
 if (input_flush_progress == true) {
        post_event ((unsigned long)buffer,0,
                OMX_COMPONENT_GENERATE_EBD);
        DEBUG_PRINT_ERROR("ERROR: ETBProxy: Input flush in progress");
 return OMX_ErrorNone;
 }
#ifdef _MSM8974_
 if (!meta_mode_enable) {
        fd = m_pInput_pmem[nBufIndex].fd;
 }
#endif
#ifdef _ANDROID_ICS_
 if (meta_mode_enable && !mUseProxyColorFormat) {
 struct pmem Input_pmem_info;
 if (!media_buffer) {
            DEBUG_PRINT_ERROR("%s: invalid media_buffer",__FUNCTION__);
 return OMX_ErrorBadParameter;
 }
 if (media_buffer->buffer_type == kMetadataBufferTypeCameraSource) {
 Input_pmem_info.buffer = media_buffer;
 Input_pmem_info.fd = media_buffer->meta_handle->data[0];
#ifdef _MSM8974_
            fd = Input_pmem_info.fd;
#endif
 Input_pmem_info.offset = media_buffer->meta_handle->data[1];
 Input_pmem_info.size = media_buffer->meta_handle->data[2];
            DEBUG_PRINT_LOW("ETB (meta-Camera) fd = %d, offset = %d, size = %d",
 Input_pmem_info.fd, Input_pmem_info.offset,
 Input_pmem_info.size);
 } else {
 private_handle_t *handle = (private_handle_t *)media_buffer->meta_handle;
 Input_pmem_info.buffer = media_buffer;
 Input_pmem_info.fd = handle->fd;
#ifdef _MSM8974_
            fd = Input_pmem_info.fd;
#endif
 Input_pmem_info.offset = 0;
 Input_pmem_info.size = handle->size;
            DEBUG_PRINT_LOW("ETB (meta-gralloc) fd = %d, offset = %d, size = %d",
 Input_pmem_info.fd, Input_pmem_info.offset,
 Input_pmem_info.size);
 }
 if (dev_use_buf(&Input_pmem_info,PORT_INDEX_IN,0) != true) {
            DEBUG_PRINT_ERROR("ERROR: in dev_use_buf");
            post_event ((unsigned long)buffer,0,OMX_COMPONENT_GENERATE_EBD);
 return OMX_ErrorBadParameter;
 }
 } else if (meta_mode_enable && !mUsesColorConversion) {
 if (media_buffer->buffer_type == kMetadataBufferTypeGrallocSource) {
 private_handle_t *handle = (private_handle_t *)media_buffer->meta_handle;
            fd = handle->fd;
            DEBUG_PRINT_LOW("ETB (opaque-gralloc) fd = %d, size = %d",
                    fd, handle->size);
 } else {
            DEBUG_PRINT_ERROR("ERROR: Invalid bufferType for buffer with Opaque"
 " color format");
            post_event ((unsigned long)buffer,0,OMX_COMPONENT_GENERATE_EBD);
 return OMX_ErrorBadParameter;
 }
 } else if (input_use_buffer && !m_use_input_pmem)
#else
 if (input_use_buffer && !m_use_input_pmem)

 #endif
     {
         DEBUG_PRINT_LOW("Heap UseBuffer case, so memcpy the data");
         pmem_data_buf = (OMX_U8 *)m_pInput_pmem[nBufIndex].buffer;
        memcpy (pmem_data_buf, (buffer->pBuffer + buffer->nOffset),
                buffer->nFilledLen);
         DEBUG_PRINT_LOW("memcpy() done in ETBProxy for i/p Heap UseBuf");
     } else if (mUseProxyColorFormat) {
        fd = m_pInput_pmem[nBufIndex].fd;
        DEBUG_PRINT_LOW("ETB (color-converted) fd = %d, size = %u",
                fd, (unsigned int)buffer->nFilledLen);
 } else if (m_sInPortDef.format.video.eColorFormat ==
                    OMX_COLOR_FormatYUV420SemiPlanar) {
 if (!dev_color_align(buffer, m_sInPortDef.format.video.nFrameWidth,
                                    m_sInPortDef.format.video.nFrameHeight)) {
                    DEBUG_PRINT_ERROR("Failed to adjust buffer color");
                    post_event((unsigned long)buffer, 0, OMX_COMPONENT_GENERATE_EBD);
 return OMX_ErrorUndefined;
 }
 }
#ifdef _MSM8974_
 if (dev_empty_buf(buffer, pmem_data_buf,nBufIndex,fd) != true)
#else
 if (dev_empty_buf(buffer, pmem_data_buf,0,0) != true)
#endif
 {
        DEBUG_PRINT_ERROR("ERROR: ETBProxy: dev_empty_buf failed");
#ifdef _ANDROID_ICS_
        omx_release_meta_buffer(buffer);
#endif
        post_event ((unsigned long)buffer,0,OMX_COMPONENT_GENERATE_EBD);
 /*Generate an async error and move to invalid state*/
        pending_input_buffers--;
 if (hw_overload) {
 return OMX_ErrorInsufficientResources;
 }
 return OMX_ErrorBadParameter;
 }
 return ret;
}
