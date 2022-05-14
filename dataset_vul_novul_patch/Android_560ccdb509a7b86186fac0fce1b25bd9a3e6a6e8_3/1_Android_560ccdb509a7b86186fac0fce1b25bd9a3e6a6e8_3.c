OMX_ERRORTYPE omx_vdec::update_portdef(OMX_PARAM_PORTDEFINITIONTYPE *portDefn)
{
    OMX_ERRORTYPE eRet = OMX_ErrorNone;
 struct v4l2_format fmt;
 if (!portDefn) {
 return OMX_ErrorBadParameter;

     }
     DEBUG_PRINT_LOW("omx_vdec::update_portdef");
     portDefn->nVersion.nVersion = OMX_SPEC_VERSION;
    portDefn->nSize = sizeof(portDefn);
     portDefn->eDomain    = OMX_PortDomainVideo;
     if (drv_ctx.frame_rate.fps_denominator > 0)
         portDefn->format.video.xFramerate = (drv_ctx.frame_rate.fps_numerator /
            drv_ctx.frame_rate.fps_denominator) << 16; //Q16 format
 else {
        DEBUG_PRINT_ERROR("Error: Divide by zero");
 return OMX_ErrorBadParameter;
 }
    memset(&fmt, 0x0, sizeof(struct v4l2_format));
 if (0 == portDefn->nPortIndex) {
        portDefn->eDir =  OMX_DirInput;
        portDefn->nBufferCountActual = drv_ctx.ip_buf.actualcount;
        portDefn->nBufferCountMin    = drv_ctx.ip_buf.mincount;
        portDefn->nBufferSize        = drv_ctx.ip_buf.buffer_size;
        portDefn->format.video.eColorFormat = OMX_COLOR_FormatUnused;
        portDefn->format.video.eCompressionFormat = eCompressionFormat;
        portDefn->bEnabled   = m_inp_bEnabled;
        portDefn->bPopulated = m_inp_bPopulated;

        fmt.type = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;
        fmt.fmt.pix_mp.pixelformat = output_capability;
 } else if (1 == portDefn->nPortIndex) {
 unsigned int buf_size = 0;
 if (!client_buffers.update_buffer_req()) {
            DEBUG_PRINT_ERROR("client_buffers.update_buffer_req Failed");
 return OMX_ErrorHardware;
 }
 if (!client_buffers.get_buffer_req(buf_size)) {
            DEBUG_PRINT_ERROR("update buffer requirements");
 return OMX_ErrorHardware;
 }
        portDefn->nBufferSize = buf_size;
        portDefn->eDir =  OMX_DirOutput;
        portDefn->nBufferCountActual = drv_ctx.op_buf.actualcount;
        portDefn->nBufferCountMin    = drv_ctx.op_buf.mincount;
        portDefn->format.video.eCompressionFormat = OMX_VIDEO_CodingUnused;
        portDefn->bEnabled   = m_out_bEnabled;
        portDefn->bPopulated = m_out_bPopulated;
 if (!client_buffers.get_color_format(portDefn->format.video.eColorFormat)) {
            DEBUG_PRINT_ERROR("Error in getting color format");
 return OMX_ErrorHardware;
 }
        fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
        fmt.fmt.pix_mp.pixelformat = capture_capability;
 } else {
        portDefn->eDir = OMX_DirMax;
        DEBUG_PRINT_LOW(" get_parameter: Bad Port idx %d",
 (int)portDefn->nPortIndex);
        eRet = OMX_ErrorBadPortIndex;
 }
 if (is_down_scalar_enabled) {
 int ret = 0;
        ret = ioctl(drv_ctx.video_driver_fd, VIDIOC_G_FMT, &fmt);
 if (ret) {
            DEBUG_PRINT_ERROR("update_portdef : Error in getting port resolution");
 return OMX_ErrorHardware;
 } else {
            portDefn->format.video.nFrameWidth = fmt.fmt.pix_mp.width;
            portDefn->format.video.nFrameHeight = fmt.fmt.pix_mp.height;
            portDefn->format.video.nStride = fmt.fmt.pix_mp.plane_fmt[0].bytesperline;
            portDefn->format.video.nSliceHeight = fmt.fmt.pix_mp.plane_fmt[0].reserved[0];
 }
 } else {
    portDefn->format.video.nFrameHeight =  drv_ctx.video_resolution.frame_height;
    portDefn->format.video.nFrameWidth  =  drv_ctx.video_resolution.frame_width;
    portDefn->format.video.nStride = drv_ctx.video_resolution.stride;
    portDefn->format.video.nSliceHeight = drv_ctx.video_resolution.scan_lines;
 }

 if ((portDefn->format.video.eColorFormat == OMX_COLOR_FormatYUV420Planar) ||
 (portDefn->format.video.eColorFormat == OMX_COLOR_FormatYUV420SemiPlanar)) {
           portDefn->format.video.nStride = ALIGN(drv_ctx.video_resolution.frame_width, 16);
        portDefn->format.video.nSliceHeight = drv_ctx.video_resolution.frame_height;
 }
    DEBUG_PRINT_HIGH("update_portdef(%u): Width = %u Height = %u Stride = %d "
 "SliceHeight = %u eColorFormat = %d nBufSize %u nBufCnt %u",
 (unsigned int)portDefn->nPortIndex,
 (unsigned int)portDefn->format.video.nFrameWidth,
 (unsigned int)portDefn->format.video.nFrameHeight,
 (int)portDefn->format.video.nStride,
 (unsigned int)portDefn->format.video.nSliceHeight,
 (unsigned int)portDefn->format.video.eColorFormat,
 (unsigned int)portDefn->nBufferSize,
 (unsigned int)portDefn->nBufferCountActual);

 return eRet;
}
