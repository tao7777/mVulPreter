OMX_ERRORTYPE  omx_vdec::set_parameter(OMX_IN OMX_HANDLETYPE     hComp,
        OMX_IN OMX_INDEXTYPE paramIndex,
        OMX_IN OMX_PTR        paramData)
{
    OMX_ERRORTYPE eRet = OMX_ErrorNone;
 int ret=0;
 struct v4l2_format fmt;
#ifdef _ANDROID_
 char property_value[PROPERTY_VALUE_MAX] = {0};
#endif
 if (m_state == OMX_StateInvalid) {
        DEBUG_PRINT_ERROR("Set Param in Invalid State");
 return OMX_ErrorInvalidState;
 }
 if (paramData == NULL) {
        DEBUG_PRINT_ERROR("Get Param in Invalid paramData");
 return OMX_ErrorBadParameter;
 }
 if ((m_state != OMX_StateLoaded) &&
            BITMASK_ABSENT(&m_flags,OMX_COMPONENT_OUTPUT_ENABLE_PENDING) &&
 (m_out_bEnabled == OMX_TRUE) &&
            BITMASK_ABSENT(&m_flags, OMX_COMPONENT_INPUT_ENABLE_PENDING) &&
 (m_inp_bEnabled == OMX_TRUE)) {
        DEBUG_PRINT_ERROR("Set Param in Invalid State");
 return OMX_ErrorIncorrectStateOperation;
 }
 switch ((unsigned long)paramIndex) {
 case OMX_IndexParamPortDefinition: {
                               VALIDATE_OMX_PARAM_DATA(paramData, OMX_PARAM_PORTDEFINITIONTYPE);
                               OMX_PARAM_PORTDEFINITIONTYPE *portDefn;
                               portDefn = (OMX_PARAM_PORTDEFINITIONTYPE *) paramData;
                               DEBUG_PRINT_LOW("set_parameter: OMX_IndexParamPortDefinition H= %d, W = %d",
 (int)portDefn->format.video.nFrameHeight,
 (int)portDefn->format.video.nFrameWidth);
 if (OMX_DirOutput == portDefn->eDir) {
                                   DEBUG_PRINT_LOW("set_parameter: OMX_IndexParamPortDefinition OP port");
 bool port_format_changed = false;
                                   m_display_id = portDefn->format.video.pNativeWindow;
 unsigned int buffer_size;
 /* update output port resolution with client supplied dimensions
                                      in case scaling is enabled, else it follows input resolution set
                                   */
 if (is_down_scalar_enabled) {
                                       DEBUG_PRINT_LOW("SetParam OP: WxH(%u x %u)",
 (unsigned int)portDefn->format.video.nFrameWidth,
 (unsigned int)portDefn->format.video.nFrameHeight);
 if (portDefn->format.video.nFrameHeight != 0x0 &&
                                               portDefn->format.video.nFrameWidth != 0x0) {
                                           memset(&fmt, 0x0, sizeof(struct v4l2_format));
                                           fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
                                           fmt.fmt.pix_mp.pixelformat = capture_capability;
                                           ret = ioctl(drv_ctx.video_driver_fd, VIDIOC_G_FMT, &fmt);
 if (ret) {
                                               DEBUG_PRINT_ERROR("Get Resolution failed");
                                               eRet = OMX_ErrorHardware;
 break;
 }
 if ((portDefn->format.video.nFrameHeight != (unsigned int)fmt.fmt.pix_mp.height) ||
 (portDefn->format.video.nFrameWidth != (unsigned int)fmt.fmt.pix_mp.width)) {
                                                   port_format_changed = true;
 }
                                           update_resolution(portDefn->format.video.nFrameWidth,
                                                   portDefn->format.video.nFrameHeight,
                                                   portDefn->format.video.nFrameWidth,
                                                   portDefn->format.video.nFrameHeight);

 /* set crop info */
                                           rectangle.nLeft = 0;
                                           rectangle.nTop = 0;
                                           rectangle.nWidth = portDefn->format.video.nFrameWidth;
                                           rectangle.nHeight = portDefn->format.video.nFrameHeight;

                                           eRet = is_video_session_supported();
 if (eRet)
 break;
                                           memset(&fmt, 0x0, sizeof(struct v4l2_format));
                                           fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
                                           fmt.fmt.pix_mp.height = drv_ctx.video_resolution.frame_height;
                                           fmt.fmt.pix_mp.width = drv_ctx.video_resolution.frame_width;
                                           fmt.fmt.pix_mp.pixelformat = capture_capability;
                                           DEBUG_PRINT_LOW("fmt.fmt.pix_mp.height = %d , fmt.fmt.pix_mp.width = %d",
                                               fmt.fmt.pix_mp.height, fmt.fmt.pix_mp.width);
                                           ret = ioctl(drv_ctx.video_driver_fd, VIDIOC_S_FMT, &fmt);
 if (ret) {
                                               DEBUG_PRINT_ERROR("Set Resolution failed");
                                               eRet = OMX_ErrorUnsupportedSetting;
 } else
                                               eRet = get_buffer_req(&drv_ctx.op_buf);
 }

 if (eRet) {
 break;
 }

 if (secure_mode) {
 struct v4l2_control control;
                                           control.id = V4L2_CID_MPEG_VIDC_VIDEO_SECURE_SCALING_THRESHOLD;
 if (ioctl(drv_ctx.video_driver_fd, VIDIOC_G_CTRL, &control) < 0) {
                                               DEBUG_PRINT_ERROR("Failed getting secure scaling threshold : %d, id was : %x", errno, control.id);
                                               eRet = OMX_ErrorHardware;
 } else {
 /* This is a workaround for a bug in fw which uses stride
                                                * and slice instead of width and height to check against
                                                * the threshold.
                                                */
                                               OMX_U32 stride, slice;
 if (drv_ctx.output_format == VDEC_YUV_FORMAT_NV12) {
                                                   stride = VENUS_Y_STRIDE(COLOR_FMT_NV12, portDefn->format.video.nFrameWidth);
                                                   slice = VENUS_Y_SCANLINES(COLOR_FMT_NV12, portDefn->format.video.nFrameHeight);
 } else {
                                                   stride = portDefn->format.video.nFrameWidth;
                                                   slice = portDefn->format.video.nFrameHeight;
 }

                                               DEBUG_PRINT_LOW("Stride is %d, slice is %d, sxs is %d\n", stride, slice, stride * slice);
                                               DEBUG_PRINT_LOW("Threshold value is %d\n", control.value);

 if (stride * slice <= (OMX_U32)control.value) {
                                                   secure_scaling_to_non_secure_opb = true;
                                                   DEBUG_PRINT_HIGH("Enabling secure scalar out of CPZ");
                                                   control.id = V4L2_CID_MPEG_VIDC_VIDEO_NON_SECURE_OUTPUT2;
                                                   control.value = 1;
 if (ioctl(drv_ctx.video_driver_fd, VIDIOC_S_CTRL, &control) < 0) {
                                                       DEBUG_PRINT_ERROR("Enabling non-secure output2 failed");
                                                       eRet = OMX_ErrorUnsupportedSetting;
 }
 }
 }
 }
 }

 if (eRet) {

                                        break;
                                    }
 
                                   if (!client_buffers.get_buffer_req(buffer_size)) {
                                        DEBUG_PRINT_ERROR("Error in getting buffer requirements");
                                        eRet = OMX_ErrorBadParameter;
                                    } else if (!port_format_changed) {
                                        if ( portDefn->nBufferCountActual >= drv_ctx.op_buf.mincount &&
                                                portDefn->nBufferSize >=  drv_ctx.op_buf.buffer_size ) {
                                            drv_ctx.op_buf.actualcount = portDefn->nBufferCountActual;
                                           drv_ctx.op_buf.buffer_size = portDefn->nBufferSize;
                                           drv_ctx.extradata_info.count = drv_ctx.op_buf.actualcount;
                                           drv_ctx.extradata_info.size = drv_ctx.extradata_info.count *
                                               drv_ctx.extradata_info.buffer_size;
                                           eRet = set_buffer_req(&drv_ctx.op_buf);
 if (eRet == OMX_ErrorNone)
                                               m_port_def = *portDefn;
 } else {
                                           DEBUG_PRINT_ERROR("ERROR: OP Requirements(#%d: %u) Requested(#%u: %u)",
                                                   drv_ctx.op_buf.mincount, (unsigned int)drv_ctx.op_buf.buffer_size,
 (unsigned int)portDefn->nBufferCountActual, (unsigned int)portDefn->nBufferSize);
                                           eRet = OMX_ErrorBadParameter;
 }
 }
 } else if (OMX_DirInput == portDefn->eDir) {
                                   DEBUG_PRINT_LOW("set_parameter: OMX_IndexParamPortDefinition IP port");
 bool port_format_changed = false;
 if ((portDefn->format.video.xFramerate >> 16) > 0 &&
 (portDefn->format.video.xFramerate >> 16) <= MAX_SUPPORTED_FPS) {
                                       DEBUG_PRINT_HIGH("set_parameter: frame rate set by omx client : %u",
 (unsigned int)portDefn->format.video.xFramerate >> 16);
                                       Q16ToFraction(portDefn->format.video.xFramerate, drv_ctx.frame_rate.fps_numerator,
                                               drv_ctx.frame_rate.fps_denominator);
 if (!drv_ctx.frame_rate.fps_numerator) {
                                           DEBUG_PRINT_ERROR("Numerator is zero setting to 30");
                                           drv_ctx.frame_rate.fps_numerator = 30;
 }
 if (drv_ctx.frame_rate.fps_denominator)
                                           drv_ctx.frame_rate.fps_numerator = (int)
                                               drv_ctx.frame_rate.fps_numerator / drv_ctx.frame_rate.fps_denominator;
                                       drv_ctx.frame_rate.fps_denominator = 1;
                                       frm_int = drv_ctx.frame_rate.fps_denominator * 1e6 /
                                           drv_ctx.frame_rate.fps_numerator;
                                       DEBUG_PRINT_LOW("set_parameter: frm_int(%u) fps(%.2f)",
 (unsigned int)frm_int, drv_ctx.frame_rate.fps_numerator /
 (float)drv_ctx.frame_rate.fps_denominator);

 struct v4l2_outputparm oparm;
 /*XXX: we're providing timing info as seconds per frame rather than frames
                                        * per second.*/
                                       oparm.timeperframe.numerator = drv_ctx.frame_rate.fps_denominator;
                                       oparm.timeperframe.denominator = drv_ctx.frame_rate.fps_numerator;

 struct v4l2_streamparm sparm;
                                       sparm.type = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;
                                       sparm.parm.output = oparm;
 if (ioctl(drv_ctx.video_driver_fd, VIDIOC_S_PARM, &sparm)) {
                                           DEBUG_PRINT_ERROR("Unable to convey fps info to driver, performance might be affected");
                                           eRet = OMX_ErrorHardware;
 break;
 }
 }

 if (drv_ctx.video_resolution.frame_height !=
                                           portDefn->format.video.nFrameHeight ||
                                           drv_ctx.video_resolution.frame_width  !=
                                           portDefn->format.video.nFrameWidth) {
                                       DEBUG_PRINT_LOW("SetParam IP: WxH(%u x %u)",
 (unsigned int)portDefn->format.video.nFrameWidth,
 (unsigned int)portDefn->format.video.nFrameHeight);
                                       port_format_changed = true;
                                       OMX_U32 frameWidth = portDefn->format.video.nFrameWidth;
                                       OMX_U32 frameHeight = portDefn->format.video.nFrameHeight;
 if (frameHeight != 0x0 && frameWidth != 0x0) {
 if (m_smoothstreaming_mode &&
 ((frameWidth * frameHeight) <
 (m_smoothstreaming_width * m_smoothstreaming_height))) {
                                               frameWidth = m_smoothstreaming_width;
                                               frameHeight = m_smoothstreaming_height;
                                               DEBUG_PRINT_LOW("NOTE: Setting resolution %u x %u "
 "for adaptive-playback/smooth-streaming",
 (unsigned int)frameWidth, (unsigned int)frameHeight);
 }
                                           update_resolution(frameWidth, frameHeight,
                                                   frameWidth, frameHeight);
                                           eRet = is_video_session_supported();
 if (eRet)
 break;
                                           memset(&fmt, 0x0, sizeof(struct v4l2_format));
                                           fmt.type = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;
                                           fmt.fmt.pix_mp.height = drv_ctx.video_resolution.frame_height;
                                           fmt.fmt.pix_mp.width = drv_ctx.video_resolution.frame_width;
                                           fmt.fmt.pix_mp.pixelformat = output_capability;
                                           DEBUG_PRINT_LOW("fmt.fmt.pix_mp.height = %d , fmt.fmt.pix_mp.width = %d",fmt.fmt.pix_mp.height,fmt.fmt.pix_mp.width);
                                           ret = ioctl(drv_ctx.video_driver_fd, VIDIOC_S_FMT, &fmt);
 if (ret) {
                                               DEBUG_PRINT_ERROR("Set Resolution failed");
                                               eRet = OMX_ErrorUnsupportedSetting;
 } else {
 if (!is_down_scalar_enabled)
                                               eRet = get_buffer_req(&drv_ctx.op_buf);
 }
 }
 }
 if (m_custom_buffersize.input_buffersize
 && (portDefn->nBufferSize > m_custom_buffersize.input_buffersize)) {
                                       DEBUG_PRINT_ERROR("ERROR: Custom buffer size set by client: %d, trying to set: %d",
                                               m_custom_buffersize.input_buffersize, portDefn->nBufferSize);

                                        eRet = OMX_ErrorBadParameter;
                                        break;
                                    }
                                    if (portDefn->nBufferCountActual >= drv_ctx.ip_buf.mincount
                                            || portDefn->nBufferSize != drv_ctx.ip_buf.buffer_size) {
                                        port_format_changed = true;
                                       vdec_allocatorproperty *buffer_prop = &drv_ctx.ip_buf;
                                       drv_ctx.ip_buf.actualcount = portDefn->nBufferCountActual;
                                       drv_ctx.ip_buf.buffer_size = (portDefn->nBufferSize + buffer_prop->alignment - 1) &
 (~(buffer_prop->alignment - 1));
                                       eRet = set_buffer_req(buffer_prop);
 }
 if (false == port_format_changed) {
                                       DEBUG_PRINT_ERROR("ERROR: IP Requirements(#%d: %u) Requested(#%u: %u)",
                                               drv_ctx.ip_buf.mincount, (unsigned int)drv_ctx.ip_buf.buffer_size,
 (unsigned int)portDefn->nBufferCountActual, (unsigned int)portDefn->nBufferSize);
                                       eRet = OMX_ErrorBadParameter;
 }
 } else if (portDefn->eDir ==  OMX_DirMax) {
                                   DEBUG_PRINT_ERROR(" Set_parameter: Bad Port idx %d",
 (int)portDefn->nPortIndex);
                                   eRet = OMX_ErrorBadPortIndex;
 }
 }
 break;
 case OMX_IndexParamVideoPortFormat: {
                                VALIDATE_OMX_PARAM_DATA(paramData, OMX_VIDEO_PARAM_PORTFORMATTYPE);
                                OMX_VIDEO_PARAM_PORTFORMATTYPE *portFmt =
 (OMX_VIDEO_PARAM_PORTFORMATTYPE *)paramData;
 int ret=0;
 struct v4l2_format fmt;
                                DEBUG_PRINT_LOW("set_parameter: OMX_IndexParamVideoPortFormat 0x%x, port: %u",
                                        portFmt->eColorFormat, (unsigned int)portFmt->nPortIndex);

                                memset(&fmt, 0x0, sizeof(struct v4l2_format));
 if (1 == portFmt->nPortIndex) {
                                    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
                                    fmt.fmt.pix_mp.height = drv_ctx.video_resolution.frame_height;
                                    fmt.fmt.pix_mp.width = drv_ctx.video_resolution.frame_width;
                                    fmt.fmt.pix_mp.pixelformat = capture_capability;
 enum vdec_output_fromat op_format;
 if (portFmt->eColorFormat == (OMX_COLOR_FORMATTYPE)
                                                QOMX_COLOR_FORMATYUV420PackedSemiPlanar32m ||
                                            portFmt->eColorFormat == (OMX_COLOR_FORMATTYPE)
                                                QOMX_COLOR_FORMATYUV420PackedSemiPlanar32mMultiView ||
                                            portFmt->eColorFormat == OMX_COLOR_FormatYUV420Planar ||
                                            portFmt->eColorFormat == OMX_COLOR_FormatYUV420SemiPlanar)
                                        op_format = (enum vdec_output_fromat)VDEC_YUV_FORMAT_NV12;
 else
                                        eRet = OMX_ErrorBadParameter;

 if (eRet == OMX_ErrorNone) {
                                        drv_ctx.output_format = op_format;
                                        ret = ioctl(drv_ctx.video_driver_fd, VIDIOC_S_FMT, &fmt);
 if (ret) {
                                            DEBUG_PRINT_ERROR("Set output format failed");
                                            eRet = OMX_ErrorUnsupportedSetting;
 /*TODO: How to handle this case */
 } else {
                                            eRet = get_buffer_req(&drv_ctx.op_buf);
 }
 }
 if (eRet == OMX_ErrorNone) {
 if (!client_buffers.set_color_format(portFmt->eColorFormat)) {
                                            DEBUG_PRINT_ERROR("Set color format failed");
                                            eRet = OMX_ErrorBadParameter;
 }
 }
 }
 }
 break;

 case OMX_QcomIndexPortDefn: {
                            VALIDATE_OMX_PARAM_DATA(paramData, OMX_QCOM_PARAM_PORTDEFINITIONTYPE);
                            OMX_QCOM_PARAM_PORTDEFINITIONTYPE *portFmt =
 (OMX_QCOM_PARAM_PORTDEFINITIONTYPE *) paramData;
                            DEBUG_PRINT_LOW("set_parameter: OMX_IndexQcomParamPortDefinitionType %u",
 (unsigned int)portFmt->nFramePackingFormat);

 /* Input port */
 if (portFmt->nPortIndex == 0) {
 if (portFmt->nFramePackingFormat == OMX_QCOM_FramePacking_Arbitrary) {
 if (secure_mode) {
                                        arbitrary_bytes = false;
                                        DEBUG_PRINT_ERROR("setparameter: cannot set to arbitary bytes mode in secure session");
                                        eRet = OMX_ErrorUnsupportedSetting;
 } else {
                                        arbitrary_bytes = true;
 }
 } else if (portFmt->nFramePackingFormat ==
                                        OMX_QCOM_FramePacking_OnlyOneCompleteFrame) {
                                    arbitrary_bytes = false;
#ifdef _ANDROID_
                                    property_get("vidc.dec.debug.arbitrarybytes.mode", property_value, "0");
 if (atoi(property_value)) {
                                        DEBUG_PRINT_HIGH("arbitrary_bytes enabled via property command");
                                        arbitrary_bytes = true;
 }
#endif
 } else {
                                    DEBUG_PRINT_ERROR("Setparameter: unknown FramePacking format %u",
 (unsigned int)portFmt->nFramePackingFormat);
                                    eRet = OMX_ErrorUnsupportedSetting;
 }
 } else if (portFmt->nPortIndex == OMX_CORE_OUTPUT_PORT_INDEX) {
                                DEBUG_PRINT_HIGH("set_parameter: OMX_IndexQcomParamPortDefinitionType OP Port");
 if ( (portFmt->nMemRegion > OMX_QCOM_MemRegionInvalid &&
                                            portFmt->nMemRegion < OMX_QCOM_MemRegionMax) &&
                                        portFmt->nCacheAttr == OMX_QCOM_CacheAttrNone) {
                                    m_out_mem_region_smi = OMX_TRUE;
 if ((m_out_mem_region_smi && m_out_pvt_entry_pmem)) {
                                        DEBUG_PRINT_HIGH("set_parameter: OMX_IndexQcomParamPortDefinitionType OP Port: out pmem set");
                                        m_use_output_pmem = OMX_TRUE;
 }
 }
 }
 }
 break;

 case OMX_IndexParamStandardComponentRole: {
                                  VALIDATE_OMX_PARAM_DATA(paramData, OMX_PARAM_COMPONENTROLETYPE);
                                  OMX_PARAM_COMPONENTROLETYPE *comp_role;
                                  comp_role = (OMX_PARAM_COMPONENTROLETYPE *) paramData;
                                  DEBUG_PRINT_LOW("set_parameter: OMX_IndexParamStandardComponentRole %s",
                                          comp_role->cRole);

 if ((m_state == OMX_StateLoaded)&&
 !BITMASK_PRESENT(&m_flags,OMX_COMPONENT_IDLE_PENDING)) {
                                      DEBUG_PRINT_LOW("Set Parameter called in valid state");
 } else {
                                      DEBUG_PRINT_ERROR("Set Parameter called in Invalid State");
 return OMX_ErrorIncorrectStateOperation;
 }

 if (!strncmp(drv_ctx.kind, "OMX.qcom.video.decoder.avc",OMX_MAX_STRINGNAME_SIZE)) {
 if (!strncmp((char*)comp_role->cRole,"video_decoder.avc",OMX_MAX_STRINGNAME_SIZE)) {
                                          strlcpy((char*)m_cRole,"video_decoder.avc",OMX_MAX_STRINGNAME_SIZE);
 } else {
                                          DEBUG_PRINT_ERROR("Setparameter: unknown Index %s", comp_role->cRole);
                                          eRet =OMX_ErrorUnsupportedSetting;
 }
 } else if (!strncmp(drv_ctx.kind, "OMX.qcom.video.decoder.mvc", OMX_MAX_STRINGNAME_SIZE)) {
 if (!strncmp((char*)comp_role->cRole, "video_decoder.mvc", OMX_MAX_STRINGNAME_SIZE)) {
                                          strlcpy((char*)m_cRole, "video_decoder.mvc", OMX_MAX_STRINGNAME_SIZE);
 } else {
                                          DEBUG_PRINT_ERROR("Setparameter: unknown Index %s", comp_role->cRole);
                                          eRet =OMX_ErrorUnsupportedSetting;
 }
 } else if (!strncmp(drv_ctx.kind, "OMX.qcom.video.decoder.mpeg4",OMX_MAX_STRINGNAME_SIZE)) {
 if (!strncmp((const char*)comp_role->cRole,"video_decoder.mpeg4",OMX_MAX_STRINGNAME_SIZE)) {
                                          strlcpy((char*)m_cRole,"video_decoder.mpeg4",OMX_MAX_STRINGNAME_SIZE);
 } else {
                                          DEBUG_PRINT_ERROR("Setparameter: unknown Index %s", comp_role->cRole);
                                          eRet = OMX_ErrorUnsupportedSetting;
 }
 } else if (!strncmp(drv_ctx.kind, "OMX.qcom.video.decoder.h263",OMX_MAX_STRINGNAME_SIZE)) {
 if (!strncmp((const char*)comp_role->cRole,"video_decoder.h263",OMX_MAX_STRINGNAME_SIZE)) {
                                          strlcpy((char*)m_cRole,"video_decoder.h263",OMX_MAX_STRINGNAME_SIZE);
 } else {
                                          DEBUG_PRINT_ERROR("Setparameter: unknown Index %s", comp_role->cRole);
                                          eRet =OMX_ErrorUnsupportedSetting;
 }
 } else if (!strncmp(drv_ctx.kind, "OMX.qcom.video.decoder.mpeg2",OMX_MAX_STRINGNAME_SIZE)) {
 if (!strncmp((const char*)comp_role->cRole,"video_decoder.mpeg2",OMX_MAX_STRINGNAME_SIZE)) {
                                          strlcpy((char*)m_cRole,"video_decoder.mpeg2",OMX_MAX_STRINGNAME_SIZE);
 } else {
                                          DEBUG_PRINT_ERROR("Setparameter: unknown Index %s", comp_role->cRole);
                                          eRet = OMX_ErrorUnsupportedSetting;
 }
 } else if ((!strncmp(drv_ctx.kind, "OMX.qcom.video.decoder.divx",OMX_MAX_STRINGNAME_SIZE)) ||
 (!strncmp(drv_ctx.kind, "OMX.qcom.video.decoder.divx311", OMX_MAX_STRINGNAME_SIZE)) ||
 (!strncmp(drv_ctx.kind, "OMX.qcom.video.decoder.divx4", OMX_MAX_STRINGNAME_SIZE))
 ) {
 if (!strncmp((const char*)comp_role->cRole,"video_decoder.divx",OMX_MAX_STRINGNAME_SIZE)) {
                                          strlcpy((char*)m_cRole,"video_decoder.divx",OMX_MAX_STRINGNAME_SIZE);
 } else {
                                          DEBUG_PRINT_ERROR("Setparameter: unknown Index %s", comp_role->cRole);
                                          eRet =OMX_ErrorUnsupportedSetting;
 }
 } else if ( (!strncmp(drv_ctx.kind, "OMX.qcom.video.decoder.vc1",OMX_MAX_STRINGNAME_SIZE)) ||
 (!strncmp(drv_ctx.kind, "OMX.qcom.video.decoder.wmv",OMX_MAX_STRINGNAME_SIZE))
 ) {
 if (!strncmp((const char*)comp_role->cRole,"video_decoder.vc1",OMX_MAX_STRINGNAME_SIZE)) {
                                          strlcpy((char*)m_cRole,"video_decoder.vc1",OMX_MAX_STRINGNAME_SIZE);
 } else {
                                          DEBUG_PRINT_ERROR("Setparameter: unknown Index %s", comp_role->cRole);
                                          eRet =OMX_ErrorUnsupportedSetting;
 }
 } else if (!strncmp(drv_ctx.kind, "OMX.qcom.video.decoder.vp8",OMX_MAX_STRINGNAME_SIZE)) {
 if (!strncmp((const char*)comp_role->cRole,"video_decoder.vp8",OMX_MAX_STRINGNAME_SIZE) ||
 (!strncmp((const char*)comp_role->cRole,"video_decoder.vpx",OMX_MAX_STRINGNAME_SIZE))) {
                                          strlcpy((char*)m_cRole,"video_decoder.vp8",OMX_MAX_STRINGNAME_SIZE);
 } else {
                                          DEBUG_PRINT_ERROR("Setparameter: unknown Index %s", comp_role->cRole);
                                          eRet = OMX_ErrorUnsupportedSetting;
 }
 } else if (!strncmp(drv_ctx.kind, "OMX.qcom.video.decoder.hevc", OMX_MAX_STRINGNAME_SIZE)) {
 if (!strncmp((const char*)comp_role->cRole, "video_decoder.hevc", OMX_MAX_STRINGNAME_SIZE)) {
                                          strlcpy((char*)m_cRole, "video_decoder.hevc", OMX_MAX_STRINGNAME_SIZE);
 } else {
                                          DEBUG_PRINT_ERROR("Setparameter: unknown Index %s", comp_role->cRole);
                                          eRet = OMX_ErrorUnsupportedSetting;
 }
 } else {
                                      DEBUG_PRINT_ERROR("Setparameter: unknown param %s", drv_ctx.kind);
                                      eRet = OMX_ErrorInvalidComponentName;
 }
 break;
 }

 case OMX_IndexParamPriorityMgmt: {
                             VALIDATE_OMX_PARAM_DATA(paramData, OMX_PRIORITYMGMTTYPE);
 if (m_state != OMX_StateLoaded) {
                                 DEBUG_PRINT_ERROR("Set Parameter called in Invalid State");
 return OMX_ErrorIncorrectStateOperation;
 }
                             OMX_PRIORITYMGMTTYPE *priorityMgmtype = (OMX_PRIORITYMGMTTYPE*) paramData;
                             DEBUG_PRINT_LOW("set_parameter: OMX_IndexParamPriorityMgmt %u",
 (unsigned int)priorityMgmtype->nGroupID);

                             DEBUG_PRINT_LOW("set_parameter: priorityMgmtype %u",
 (unsigned int)priorityMgmtype->nGroupPriority);

                             m_priority_mgm.nGroupID = priorityMgmtype->nGroupID;
                             m_priority_mgm.nGroupPriority = priorityMgmtype->nGroupPriority;

 break;
 }

 case OMX_IndexParamCompBufferSupplier: {
                                   VALIDATE_OMX_PARAM_DATA(paramData, OMX_PARAM_BUFFERSUPPLIERTYPE);
                                   OMX_PARAM_BUFFERSUPPLIERTYPE *bufferSupplierType = (OMX_PARAM_BUFFERSUPPLIERTYPE*) paramData;
                                   DEBUG_PRINT_LOW("set_parameter: OMX_IndexParamCompBufferSupplier %d",
                                           bufferSupplierType->eBufferSupplier);
 if (bufferSupplierType->nPortIndex == 0 || bufferSupplierType->nPortIndex ==1)
                                       m_buffer_supplier.eBufferSupplier = bufferSupplierType->eBufferSupplier;

 else

                                       eRet = OMX_ErrorBadPortIndex;

 break;

 }
 case OMX_IndexParamVideoAvc: {
                             DEBUG_PRINT_LOW("set_parameter: OMX_IndexParamVideoAvc %d",
                                     paramIndex);
 break;
 }
 case (OMX_INDEXTYPE)QOMX_IndexParamVideoMvc: {
                            DEBUG_PRINT_LOW("set_parameter: QOMX_IndexParamVideoMvc %d",
                                     paramIndex);
 break;
 }
 case OMX_IndexParamVideoH263: {
                              DEBUG_PRINT_LOW("set_parameter: OMX_IndexParamVideoH263 %d",
                                      paramIndex);
 break;
 }
 case OMX_IndexParamVideoMpeg4: {
                               DEBUG_PRINT_LOW("set_parameter: OMX_IndexParamVideoMpeg4 %d",
                                       paramIndex);
 break;
 }
 case OMX_IndexParamVideoMpeg2: {
                               DEBUG_PRINT_LOW("set_parameter: OMX_IndexParamVideoMpeg2 %d",
                                       paramIndex);
 break;
 }
 case OMX_QcomIndexParamVideoDecoderPictureOrder: {
                                     VALIDATE_OMX_PARAM_DATA(paramData, QOMX_VIDEO_DECODER_PICTURE_ORDER);
                                     QOMX_VIDEO_DECODER_PICTURE_ORDER *pictureOrder =
 (QOMX_VIDEO_DECODER_PICTURE_ORDER *)paramData;
 struct v4l2_control control;
 int pic_order,rc=0;
                                     DEBUG_PRINT_HIGH("set_parameter: OMX_QcomIndexParamVideoDecoderPictureOrder %d",
                                             pictureOrder->eOutputPictureOrder);
 if (pictureOrder->eOutputPictureOrder == QOMX_VIDEO_DISPLAY_ORDER) {
                                         pic_order = V4L2_MPEG_VIDC_VIDEO_OUTPUT_ORDER_DISPLAY;
 } else if (pictureOrder->eOutputPictureOrder == QOMX_VIDEO_DECODE_ORDER) {
                                         pic_order = V4L2_MPEG_VIDC_VIDEO_OUTPUT_ORDER_DECODE;
                                         time_stamp_dts.set_timestamp_reorder_mode(false);
 } else
                                         eRet = OMX_ErrorBadParameter;
 if (eRet == OMX_ErrorNone) {
                                         control.id = V4L2_CID_MPEG_VIDC_VIDEO_OUTPUT_ORDER;
                                         control.value = pic_order;
                                         rc = ioctl(drv_ctx.video_driver_fd, VIDIOC_S_CTRL, &control);
 if (rc) {
                                             DEBUG_PRINT_ERROR("Set picture order failed");
                                             eRet = OMX_ErrorUnsupportedSetting;
 }
 }
 break;
 }
 case OMX_QcomIndexParamConcealMBMapExtraData:
                               VALIDATE_OMX_PARAM_DATA(paramData, QOMX_ENABLETYPE);
                                     eRet = enable_extradata(VDEC_EXTRADATA_MB_ERROR_MAP, false,
 ((QOMX_ENABLETYPE *)paramData)->bEnable);
 break;
 case OMX_QcomIndexParamFrameInfoExtraData:
                               VALIDATE_OMX_PARAM_DATA(paramData, QOMX_ENABLETYPE);
                                       eRet = enable_extradata(OMX_FRAMEINFO_EXTRADATA, false,
 ((QOMX_ENABLETYPE *)paramData)->bEnable);
 break;
 case OMX_ExtraDataFrameDimension:
                               VALIDATE_OMX_PARAM_DATA(paramData, QOMX_ENABLETYPE);
                               eRet = enable_extradata(OMX_FRAMEDIMENSION_EXTRADATA, false,
 ((QOMX_ENABLETYPE *)paramData)->bEnable);
 break;
 case OMX_QcomIndexParamInterlaceExtraData:
                               VALIDATE_OMX_PARAM_DATA(paramData, QOMX_ENABLETYPE);
                                   eRet = enable_extradata(OMX_INTERLACE_EXTRADATA, false,
 ((QOMX_ENABLETYPE *)paramData)->bEnable);
 break;
 case OMX_QcomIndexParamH264TimeInfo:
                               VALIDATE_OMX_PARAM_DATA(paramData, QOMX_ENABLETYPE);
                                   eRet = enable_extradata(OMX_TIMEINFO_EXTRADATA, false,
 ((QOMX_ENABLETYPE *)paramData)->bEnable);
 break;
 case OMX_QcomIndexParamVideoFramePackingExtradata:
                               VALIDATE_OMX_PARAM_DATA(paramData, QOMX_ENABLETYPE);
                               eRet = enable_extradata(OMX_FRAMEPACK_EXTRADATA, false,
 ((QOMX_ENABLETYPE *)paramData)->bEnable);
 break;
 case OMX_QcomIndexParamVideoQPExtraData:
                               VALIDATE_OMX_PARAM_DATA(paramData, QOMX_ENABLETYPE);
                               eRet = enable_extradata(OMX_QP_EXTRADATA, false,
 ((QOMX_ENABLETYPE *)paramData)->bEnable);
 break;
 case OMX_QcomIndexParamVideoInputBitsInfoExtraData:
                               VALIDATE_OMX_PARAM_DATA(paramData, QOMX_ENABLETYPE);
                               eRet = enable_extradata(OMX_BITSINFO_EXTRADATA, false,
 ((QOMX_ENABLETYPE *)paramData)->bEnable);
 break;
 case OMX_QcomIndexEnableExtnUserData:
                               VALIDATE_OMX_PARAM_DATA(paramData, QOMX_ENABLETYPE);
                                eRet = enable_extradata(OMX_EXTNUSER_EXTRADATA, false,
 ((QOMX_ENABLETYPE *)paramData)->bEnable);
 break;
 case OMX_QcomIndexParamMpeg2SeqDispExtraData:
                               VALIDATE_OMX_PARAM_DATA(paramData, QOMX_ENABLETYPE);
                                eRet = enable_extradata(OMX_MPEG2SEQDISP_EXTRADATA, false,
 ((QOMX_ENABLETYPE *)paramData)->bEnable);
 break;
 case OMX_QcomIndexParamVideoDivx: {
                              QOMX_VIDEO_PARAM_DIVXTYPE* divXType = (QOMX_VIDEO_PARAM_DIVXTYPE *) paramData;
 }
 break;
 case OMX_QcomIndexPlatformPvt: {
                               VALIDATE_OMX_PARAM_DATA(paramData, OMX_QCOM_PLATFORMPRIVATE_EXTN);
                               DEBUG_PRINT_HIGH("set_parameter: OMX_QcomIndexPlatformPvt OP Port");
                               OMX_QCOM_PLATFORMPRIVATE_EXTN* entryType = (OMX_QCOM_PLATFORMPRIVATE_EXTN *) paramData;
 if (entryType->type != OMX_QCOM_PLATFORM_PRIVATE_PMEM) {
                                   DEBUG_PRINT_HIGH("set_parameter: Platform Private entry type (%d) not supported.", entryType->type);
                                   eRet = OMX_ErrorUnsupportedSetting;
 } else {
                                   m_out_pvt_entry_pmem = OMX_TRUE;
 if ((m_out_mem_region_smi && m_out_pvt_entry_pmem)) {
                                       DEBUG_PRINT_HIGH("set_parameter: OMX_QcomIndexPlatformPvt OP Port: out pmem set");
                                       m_use_output_pmem = OMX_TRUE;
 }
 }

 }
 break;
 case OMX_QcomIndexParamVideoSyncFrameDecodingMode: {
                                       DEBUG_PRINT_HIGH("set_parameter: OMX_QcomIndexParamVideoSyncFrameDecodingMode");
                                       DEBUG_PRINT_HIGH("set idr only decoding for thumbnail mode");
 struct v4l2_control control;
 int rc;
                                       drv_ctx.idr_only_decoding = 1;
                                       control.id = V4L2_CID_MPEG_VIDC_VIDEO_OUTPUT_ORDER;
                                       control.value = V4L2_MPEG_VIDC_VIDEO_OUTPUT_ORDER_DECODE;
                                       rc = ioctl(drv_ctx.video_driver_fd, VIDIOC_S_CTRL, &control);
 if (rc) {
                                           DEBUG_PRINT_ERROR("Set picture order failed");
                                           eRet = OMX_ErrorUnsupportedSetting;
 } else {
                                           control.id = V4L2_CID_MPEG_VIDC_VIDEO_SYNC_FRAME_DECODE;
                                           control.value = V4L2_MPEG_VIDC_VIDEO_SYNC_FRAME_DECODE_ENABLE;
                                           rc = ioctl(drv_ctx.video_driver_fd, VIDIOC_S_CTRL, &control);
 if (rc) {
                                               DEBUG_PRINT_ERROR("Sync frame setting failed");
                                               eRet = OMX_ErrorUnsupportedSetting;
 }
 /*Setting sync frame decoding on driver might change buffer
                                            * requirements so update them here*/
 if (get_buffer_req(&drv_ctx.ip_buf)) {
                                               DEBUG_PRINT_ERROR("Sync frame setting failed: falied to get buffer i/p requirements");
                                               eRet = OMX_ErrorUnsupportedSetting;
 }
 if (get_buffer_req(&drv_ctx.op_buf)) {
                                               DEBUG_PRINT_ERROR("Sync frame setting failed: falied to get buffer o/p requirements");
                                               eRet = OMX_ErrorUnsupportedSetting;
 }
 }
 }
 break;

 case OMX_QcomIndexParamIndexExtraDataType: {
                                    VALIDATE_OMX_PARAM_DATA(paramData, QOMX_INDEXEXTRADATATYPE);
                                       QOMX_INDEXEXTRADATATYPE *extradataIndexType = (QOMX_INDEXEXTRADATATYPE *) paramData;
 if ((extradataIndexType->nIndex == OMX_IndexParamPortDefinition) &&
 (extradataIndexType->bEnabled == OMX_TRUE) &&
 (extradataIndexType->nPortIndex == 1)) {
                                        DEBUG_PRINT_HIGH("set_parameter:  OMX_QcomIndexParamIndexExtraDataType SmoothStreaming");
                                           eRet = enable_extradata(OMX_PORTDEF_EXTRADATA, false, extradataIndexType->bEnabled);

 }
 }
 break;
 case OMX_QcomIndexParamEnableSmoothStreaming: {
#ifndef SMOOTH_STREAMING_DISABLED
                                      eRet = enable_smoothstreaming();
#else
                                      eRet = OMX_ErrorUnsupportedSetting;
#endif
 }
 break;
#if defined (_ANDROID_HONEYCOMB_) || defined (_ANDROID_ICS_)
 /* Need to allow following two set_parameters even in Idle
                                   * state. This is ANDROID architecture which is not in sync
                                   * with openmax standard. */
 case OMX_GoogleAndroidIndexEnableAndroidNativeBuffers: {
                                           VALIDATE_OMX_PARAM_DATA(paramData, EnableAndroidNativeBuffersParams);
 EnableAndroidNativeBuffersParams* enableNativeBuffers = (EnableAndroidNativeBuffersParams *) paramData;
 if (enableNativeBuffers) {
                                               m_enable_android_native_buffers = enableNativeBuffers->enable;
 }
#if !defined(FLEXYUV_SUPPORTED)
 if (m_enable_android_native_buffers) {
 if(!client_buffers.set_color_format(getPreferredColorFormatDefaultMode(0))) {
                                                   DEBUG_PRINT_ERROR("Failed to set native color format!");
                                                   eRet = OMX_ErrorUnsupportedSetting;
 }
 }
#endif
 }
 break;
 case OMX_GoogleAndroidIndexUseAndroidNativeBuffer: {
                                       VALIDATE_OMX_PARAM_DATA(paramData, UseAndroidNativeBufferParams);
                                       eRet = use_android_native_buffer(hComp, paramData);
 }
 break;
#endif
 case OMX_QcomIndexParamEnableTimeStampReorder: {
                                       VALIDATE_OMX_PARAM_DATA(paramData, QOMX_INDEXTIMESTAMPREORDER);
                                       QOMX_INDEXTIMESTAMPREORDER *reorder = (QOMX_INDEXTIMESTAMPREORDER *)paramData;
 if (drv_ctx.picture_order == (vdec_output_order)QOMX_VIDEO_DISPLAY_ORDER) {
 if (reorder->bEnable == OMX_TRUE) {
                                               frm_int =0;
                                               time_stamp_dts.set_timestamp_reorder_mode(true);
 } else
                                               time_stamp_dts.set_timestamp_reorder_mode(false);
 } else {
                                           time_stamp_dts.set_timestamp_reorder_mode(false);
 if (reorder->bEnable == OMX_TRUE) {
                                               eRet = OMX_ErrorUnsupportedSetting;
 }
 }
 }
 break;
 case OMX_IndexParamVideoProfileLevelCurrent: {
                                     VALIDATE_OMX_PARAM_DATA(paramData, OMX_VIDEO_PARAM_PROFILELEVELTYPE);
                                     OMX_VIDEO_PARAM_PROFILELEVELTYPE* pParam =
 (OMX_VIDEO_PARAM_PROFILELEVELTYPE*)paramData;
 if (pParam) {
                                         m_profile_lvl.eProfile = pParam->eProfile;
                                         m_profile_lvl.eLevel = pParam->eLevel;
 }
 break;

 }
 case OMX_QcomIndexParamVideoMetaBufferMode:
 {
            VALIDATE_OMX_PARAM_DATA(paramData, StoreMetaDataInBuffersParams);
 StoreMetaDataInBuffersParams *metabuffer =
 (StoreMetaDataInBuffersParams *)paramData;
 if (!metabuffer) {
                DEBUG_PRINT_ERROR("Invalid param: %p", metabuffer);
                eRet = OMX_ErrorBadParameter;
 break;
 }
 if (m_disable_dynamic_buf_mode) {
                DEBUG_PRINT_HIGH("Dynamic buffer mode disabled by setprop");
                eRet = OMX_ErrorUnsupportedSetting;
 break;
 }
 if (metabuffer->nPortIndex == OMX_CORE_OUTPUT_PORT_INDEX) {
 struct v4l2_control control;
 struct v4l2_format fmt;
                    control.id = V4L2_CID_MPEG_VIDC_VIDEO_ALLOC_MODE_OUTPUT;
 if (metabuffer->bStoreMetaData == true) {
                    control.value = V4L2_MPEG_VIDC_VIDEO_DYNAMIC;
 } else {
                        control.value = V4L2_MPEG_VIDC_VIDEO_STATIC;
 }
 int rc = ioctl(drv_ctx.video_driver_fd, VIDIOC_S_CTRL,&control);
 if (!rc) {
                        DEBUG_PRINT_HIGH("%s buffer mode",
 (metabuffer->bStoreMetaData == true)? "Enabled dynamic" : "Disabled dynamic");
                               dynamic_buf_mode = metabuffer->bStoreMetaData;
 } else {
                        DEBUG_PRINT_ERROR("Failed to %s buffer mode",
 (metabuffer->bStoreMetaData == true)? "enable dynamic" : "disable dynamic");
                        eRet = OMX_ErrorUnsupportedSetting;
 }
 } else {
                    DEBUG_PRINT_ERROR(
 "OMX_QcomIndexParamVideoMetaBufferMode not supported for port: %u",
 (unsigned int)metabuffer->nPortIndex);
                    eRet = OMX_ErrorUnsupportedSetting;
 }
 break;
 }
 case OMX_QcomIndexParamVideoDownScalar:
 {
            VALIDATE_OMX_PARAM_DATA(paramData, QOMX_INDEXDOWNSCALAR);
            QOMX_INDEXDOWNSCALAR* pParam = (QOMX_INDEXDOWNSCALAR*)paramData;
 struct v4l2_control control;
 int rc;
 if (pParam) {
                is_down_scalar_enabled = pParam->bEnable;
 if (is_down_scalar_enabled) {
                    control.id = V4L2_CID_MPEG_VIDC_VIDEO_STREAM_OUTPUT_MODE;
                    control.value = V4L2_CID_MPEG_VIDC_VIDEO_STREAM_OUTPUT_SECONDARY;
                    DEBUG_PRINT_LOW("set_parameter:  OMX_QcomIndexParamVideoDownScalar value = %d", pParam->bEnable);
                    rc = ioctl(drv_ctx.video_driver_fd, VIDIOC_S_CTRL, &control);
 if (rc < 0) {
                        DEBUG_PRINT_ERROR("Failed to set down scalar on driver.");
                        eRet = OMX_ErrorUnsupportedSetting;
 }
                    control.id = V4L2_CID_MPEG_VIDC_VIDEO_KEEP_ASPECT_RATIO;
                    control.value = 1;
                    rc = ioctl(drv_ctx.video_driver_fd, VIDIOC_S_CTRL, &control);
 if (rc < 0) {
                        DEBUG_PRINT_ERROR("Failed to set keep aspect ratio on driver.");
                        eRet = OMX_ErrorUnsupportedSetting;
 }
 }
 }
 break;
 }
#ifdef ADAPTIVE_PLAYBACK_SUPPORTED
 case OMX_QcomIndexParamVideoAdaptivePlaybackMode:
 {
            VALIDATE_OMX_PARAM_DATA(paramData, PrepareForAdaptivePlaybackParams);
            DEBUG_PRINT_LOW("set_parameter: OMX_GoogleAndroidIndexPrepareForAdaptivePlayback");
 PrepareForAdaptivePlaybackParams* pParams =
 (PrepareForAdaptivePlaybackParams *) paramData;
 if (pParams->nPortIndex == OMX_CORE_OUTPUT_PORT_INDEX) {
 if (!pParams->bEnable) {
 return OMX_ErrorNone;
 }
 if (pParams->nMaxFrameWidth > maxSmoothStreamingWidth
 || pParams->nMaxFrameHeight > maxSmoothStreamingHeight) {
                    DEBUG_PRINT_ERROR(
 "Adaptive playback request exceeds max supported resolution : [%u x %u] vs [%u x %u]",
 (unsigned int)pParams->nMaxFrameWidth, (unsigned int)pParams->nMaxFrameHeight,
 (unsigned int)maxSmoothStreamingWidth, (unsigned int)maxSmoothStreamingHeight);
                    eRet = OMX_ErrorBadParameter;
 } else {
                    eRet = enable_adaptive_playback(pParams->nMaxFrameWidth, pParams->nMaxFrameHeight);
 }
 } else {
                DEBUG_PRINT_ERROR(
 "Prepare for adaptive playback supported only on output port");
                eRet = OMX_ErrorBadParameter;
 }
 break;
 }

#endif
 case OMX_QcomIndexParamVideoCustomBufferSize:
 {
            VALIDATE_OMX_PARAM_DATA(paramData, QOMX_VIDEO_CUSTOM_BUFFERSIZE);
            DEBUG_PRINT_LOW("set_parameter: OMX_QcomIndexParamVideoCustomBufferSize");
            QOMX_VIDEO_CUSTOM_BUFFERSIZE* pParam = (QOMX_VIDEO_CUSTOM_BUFFERSIZE*)paramData;
 if (pParam->nPortIndex == OMX_CORE_INPUT_PORT_INDEX) {
 struct v4l2_control control;
                control.id = V4L2_CID_MPEG_VIDC_VIDEO_BUFFER_SIZE_LIMIT;
                control.value = pParam->nBufferSize;
 if (ioctl(drv_ctx.video_driver_fd, VIDIOC_S_CTRL, &control)) {
                    DEBUG_PRINT_ERROR("Failed to set input buffer size");
                    eRet = OMX_ErrorUnsupportedSetting;
 } else {
                    eRet = get_buffer_req(&drv_ctx.ip_buf);
 if (eRet == OMX_ErrorNone) {
                        m_custom_buffersize.input_buffersize = drv_ctx.ip_buf.buffer_size;
                        DEBUG_PRINT_HIGH("Successfully set custom input buffer size = %d",
                            m_custom_buffersize.input_buffersize);
 } else {
                        DEBUG_PRINT_ERROR("Failed to get buffer requirement");
 }
 }
 } else {
                DEBUG_PRINT_ERROR("ERROR: Custom buffer size in not supported on output port");
                eRet = OMX_ErrorBadParameter;
 }
 break;
 }
 default: {
                 DEBUG_PRINT_ERROR("Setparameter: unknown param %d", paramIndex);
                 eRet = OMX_ErrorUnsupportedIndex;
 }
 }
 if (eRet != OMX_ErrorNone)
        DEBUG_PRINT_ERROR("set_parameter: Error: 0x%x, setting param 0x%x", eRet, paramIndex);
 return eRet;
}
