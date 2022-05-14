OMX_ERRORTYPE  omx_vdec::empty_this_buffer_proxy(OMX_IN OMX_HANDLETYPE  hComp,
        OMX_IN OMX_BUFFERHEADERTYPE* buffer)
{
 (void) hComp;
 int push_cnt = 0,i=0;
 unsigned nPortIndex = 0;
    OMX_ERRORTYPE ret = OMX_ErrorNone;
 struct vdec_input_frameinfo frameinfo;
 struct vdec_bufferpayload *temp_buffer;
 struct vdec_seqheader seq_header;
 bool port_setting_changed = true;

 /*Should we generate a Aync error event*/
 if (buffer == NULL || buffer->pInputPortPrivate == NULL) {
        DEBUG_PRINT_ERROR("ERROR:empty_this_buffer_proxy is invalid");
 return OMX_ErrorBadParameter;
 }

    nPortIndex = buffer-((OMX_BUFFERHEADERTYPE *)m_inp_mem_ptr);

 if (nPortIndex > drv_ctx.ip_buf.actualcount) {
        DEBUG_PRINT_ERROR("ERROR:empty_this_buffer_proxy invalid nPortIndex[%u]",
                nPortIndex);
 return OMX_ErrorBadParameter;
 }

    pending_input_buffers++;

 /* return zero length and not an EOS buffer */
 if (!arbitrary_bytes && (buffer->nFilledLen == 0) &&
 ((buffer->nFlags & OMX_BUFFERFLAG_EOS) == 0)) {
        DEBUG_PRINT_HIGH("return zero legth buffer");
        post_event ((unsigned long)buffer,VDEC_S_SUCCESS,
                OMX_COMPONENT_GENERATE_EBD);
 return OMX_ErrorNone;
 }

 if (input_flush_progress == true) {
        DEBUG_PRINT_LOW("Flush in progress return buffer ");
        post_event ((unsigned long)buffer,VDEC_S_SUCCESS,
                OMX_COMPONENT_GENERATE_EBD);

         return OMX_ErrorNone;
     }
 
     temp_buffer = (struct vdec_bufferpayload *)buffer->pInputPortPrivate;
 
    if ((temp_buffer -  drv_ctx.ptr_inputbuffer) > (int)drv_ctx.ip_buf.actualcount) {
         return OMX_ErrorBadParameter;
     }
     /* If its first frame, H264 codec and reject is true, then parse the nal
       and get the profile. Based on this, reject the clip playback */
 if (first_frame == 0 && codec_type_parse == CODEC_TYPE_H264 &&
            m_reject_avc_1080p_mp) {
        first_frame = 1;
        DEBUG_PRINT_ERROR("Parse nal to get the profile");
        h264_parser->parse_nal((OMX_U8*)buffer->pBuffer, buffer->nFilledLen,
                NALU_TYPE_SPS);
        m_profile = h264_parser->get_profile();
        ret = is_video_session_supported();
 if (ret) {
            post_event ((unsigned long)buffer,VDEC_S_SUCCESS,OMX_COMPONENT_GENERATE_EBD);
            post_event(OMX_EventError, OMX_ErrorInvalidState,OMX_COMPONENT_GENERATE_EVENT);
 /* Move the state to Invalid to avoid queueing of pending ETB to the driver */
            m_state = OMX_StateInvalid;
 return OMX_ErrorNone;
 }
 }

    DEBUG_PRINT_LOW("ETBProxy: bufhdr = %p, bufhdr->pBuffer = %p", buffer, buffer->pBuffer);

     /*for use buffer we need to memcpy the data*/
     temp_buffer->buffer_len = buffer->nFilledLen;
 
    if (input_use_buffer) {
         if (buffer->nFilledLen <= temp_buffer->buffer_len) {
             if (arbitrary_bytes) {
                 memcpy (temp_buffer->bufferaddr, (buffer->pBuffer + buffer->nOffset),buffer->nFilledLen);
 } else {
                memcpy (temp_buffer->bufferaddr, (m_inp_heap_ptr[nPortIndex].pBuffer + m_inp_heap_ptr[nPortIndex].nOffset),
                        buffer->nFilledLen);
 }
 } else {
 return OMX_ErrorBadParameter;
 }

 }

    frameinfo.bufferaddr = temp_buffer->bufferaddr;
    frameinfo.client_data = (void *) buffer;
    frameinfo.datalen = temp_buffer->buffer_len;
    frameinfo.flags = 0;
    frameinfo.offset = buffer->nOffset;
    frameinfo.pmem_fd = temp_buffer->pmem_fd;
    frameinfo.pmem_offset = temp_buffer->offset;
    frameinfo.timestamp = buffer->nTimeStamp;
 if (drv_ctx.disable_dmx && m_desc_buffer_ptr && m_desc_buffer_ptr[nPortIndex].buf_addr) {
        DEBUG_PRINT_LOW("ETB: dmx enabled");
 if (m_demux_entries == 0) {
            extract_demux_addr_offsets(buffer);
 }

        DEBUG_PRINT_LOW("ETB: handle_demux_data - entries=%u",(unsigned int)m_demux_entries);
        handle_demux_data(buffer);
        frameinfo.desc_addr = (OMX_U8 *)m_desc_buffer_ptr[nPortIndex].buf_addr;
        frameinfo.desc_size = m_desc_buffer_ptr[nPortIndex].desc_data_size;
 } else {
        frameinfo.desc_addr = NULL;
        frameinfo.desc_size = 0;
 }
 if (!arbitrary_bytes) {
        frameinfo.flags |= buffer->nFlags;
 }

#ifdef _ANDROID_
 if (m_debug_timestamp) {
 if (arbitrary_bytes) {
            DEBUG_PRINT_LOW("Inserting TIMESTAMP (%lld) into queue", buffer->nTimeStamp);
            m_timestamp_list.insert_ts(buffer->nTimeStamp);
 } else if (!arbitrary_bytes && !(buffer->nFlags & OMX_BUFFERFLAG_CODECCONFIG)) {
            DEBUG_PRINT_LOW("Inserting TIMESTAMP (%lld) into queue", buffer->nTimeStamp);
            m_timestamp_list.insert_ts(buffer->nTimeStamp);
 }
 }
#endif

log_input_buffers((const char *)temp_buffer->bufferaddr, temp_buffer->buffer_len);

 if (buffer->nFlags & QOMX_VIDEO_BUFFERFLAG_EOSEQ) {
        frameinfo.flags |= QOMX_VIDEO_BUFFERFLAG_EOSEQ;
        buffer->nFlags &= ~QOMX_VIDEO_BUFFERFLAG_EOSEQ;
 }

 if (temp_buffer->buffer_len == 0 || (buffer->nFlags & OMX_BUFFERFLAG_EOS)) {
        DEBUG_PRINT_HIGH("Rxd i/p EOS, Notify Driver that EOS has been reached");
        frameinfo.flags |= VDEC_BUFFERFLAG_EOS;
        h264_scratch.nFilledLen = 0;
        nal_count = 0;
        look_ahead_nal = false;
        frame_count = 0;
 if (m_frame_parser.mutils)
            m_frame_parser.mutils->initialize_frame_checking_environment();
        m_frame_parser.flush();
        h264_last_au_ts = LLONG_MAX;
        h264_last_au_flags = 0;
        memset(m_demux_offsets, 0, ( sizeof(OMX_U32) * 8192) );
        m_demux_entries = 0;
 }
 struct v4l2_buffer buf;
 struct v4l2_plane plane;
    memset( (void *)&buf, 0, sizeof(buf));
    memset( (void *)&plane, 0, sizeof(plane));
 int rc;
 unsigned long  print_count;
 if (temp_buffer->buffer_len == 0 || (buffer->nFlags & OMX_BUFFERFLAG_EOS)) {
        buf.flags = V4L2_QCOM_BUF_FLAG_EOS;
        DEBUG_PRINT_HIGH("INPUT EOS reached") ;
 }
    OMX_ERRORTYPE eRet = OMX_ErrorNone;
    buf.index = nPortIndex;
    buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;
    buf.memory = V4L2_MEMORY_USERPTR;
    plane.bytesused = temp_buffer->buffer_len;
    plane.length = drv_ctx.ip_buf.buffer_size;
    plane.m.userptr = (unsigned long)temp_buffer->bufferaddr -
 (unsigned long)temp_buffer->offset;
    plane.reserved[0] = temp_buffer->pmem_fd;
    plane.reserved[1] = temp_buffer->offset;
    plane.data_offset = 0;
    buf.m.planes = &plane;
    buf.length = 1;
 if (frameinfo.timestamp >= LLONG_MAX) {
        buf.flags |= V4L2_QCOM_BUF_TIMESTAMP_INVALID;
 }
    buf.timestamp.tv_sec = frameinfo.timestamp / 1000000;
    buf.timestamp.tv_usec = (frameinfo.timestamp % 1000000);
    buf.flags |= (buffer->nFlags & OMX_BUFFERFLAG_CODECCONFIG) ? V4L2_QCOM_BUF_FLAG_CODECCONFIG: 0;
    buf.flags |= (buffer->nFlags & OMX_BUFFERFLAG_DECODEONLY) ? V4L2_QCOM_BUF_FLAG_DECODEONLY: 0;

 if (buffer->nFlags & OMX_BUFFERFLAG_CODECCONFIG) {
        DEBUG_PRINT_LOW("Increment codec_config buffer counter");
        android_atomic_inc(&m_queued_codec_config_count);
 }

    rc = ioctl(drv_ctx.video_driver_fd, VIDIOC_QBUF, &buf);
 if (rc) {
        DEBUG_PRINT_ERROR("Failed to qbuf Input buffer to driver");
 return OMX_ErrorHardware;
 }

 if (codec_config_flag && !(buffer->nFlags & OMX_BUFFERFLAG_CODECCONFIG)) {
        codec_config_flag = false;
 }
 if (!streaming[OUTPUT_PORT]) {
 enum v4l2_buf_type buf_type;
 int ret,r;

        buf_type=V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;
        DEBUG_PRINT_LOW("send_command_proxy(): Idle-->Executing");
        ret=ioctl(drv_ctx.video_driver_fd, VIDIOC_STREAMON,&buf_type);
 if (!ret) {
            DEBUG_PRINT_HIGH("Streamon on OUTPUT Plane was successful");
            streaming[OUTPUT_PORT] = true;
 } else if (errno == EBUSY) {
            DEBUG_PRINT_ERROR("Failed to call stream on OUTPUT due to HW_OVERLOAD");
            post_event ((unsigned long)buffer, VDEC_S_SUCCESS,
                    OMX_COMPONENT_GENERATE_EBD);
 return OMX_ErrorInsufficientResources;
 } else {
            DEBUG_PRINT_ERROR("Failed to call streamon on OUTPUT");
            DEBUG_PRINT_LOW("If Stream on failed no buffer should be queued");
            post_event ((unsigned long)buffer,VDEC_S_SUCCESS,
                    OMX_COMPONENT_GENERATE_EBD);
 return OMX_ErrorBadParameter;
 }
 }
    DEBUG_PRINT_LOW("[ETBP] pBuf(%p) nTS(%lld) Sz(%u)",
            frameinfo.bufferaddr, (long long)frameinfo.timestamp,
 (unsigned int)frameinfo.datalen);

 return ret;
}
