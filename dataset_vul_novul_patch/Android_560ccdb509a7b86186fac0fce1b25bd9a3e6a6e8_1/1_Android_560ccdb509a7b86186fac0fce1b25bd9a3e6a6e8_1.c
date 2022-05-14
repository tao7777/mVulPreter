OMX_ERRORTYPE  omx_vdec::set_config(OMX_IN OMX_HANDLETYPE      hComp,
        OMX_IN OMX_INDEXTYPE configIndex,
        OMX_IN OMX_PTR        configData)
{
 (void) hComp;
 if (m_state == OMX_StateInvalid) {
        DEBUG_PRINT_ERROR("Get Config in Invalid State");
 return OMX_ErrorInvalidState;
 }

    OMX_ERRORTYPE ret = OMX_ErrorNone;
    OMX_VIDEO_CONFIG_NALSIZE *pNal;

    DEBUG_PRINT_LOW("Set Config Called");

 if (configIndex == (OMX_INDEXTYPE)OMX_IndexVendorVideoExtraData) {
        OMX_VENDOR_EXTRADATATYPE *config = (OMX_VENDOR_EXTRADATATYPE *) configData;
        DEBUG_PRINT_LOW("Index OMX_IndexVendorVideoExtraData called");
 if (!strcmp(drv_ctx.kind, "OMX.qcom.video.decoder.avc") ||
 !strcmp(drv_ctx.kind, "OMX.qcom.video.decoder.mvc")) {
            DEBUG_PRINT_LOW("Index OMX_IndexVendorVideoExtraData AVC");
            OMX_U32 extra_size;

            nal_length = (config->pData[4] & 0x03) + 1;

            extra_size = 0;
 if (nal_length > 2) {
 /* Presently we assume that only one SPS and one PPS in AvC1 Atom */
                extra_size = (nal_length - 2) * 2;
 }

            OMX_U8 *pSrcBuf = (OMX_U8 *) (&config->pData[6]);
            OMX_U8 *pDestBuf;
            m_vendor_config.nPortIndex = config->nPortIndex;

            m_vendor_config.nDataSize = config->nDataSize - 6 - 1 + extra_size;
            m_vendor_config.pData = (OMX_U8 *) malloc(m_vendor_config.nDataSize);
            OMX_U32 len;
            OMX_U8 index = 0;
            pDestBuf = m_vendor_config.pData;

            DEBUG_PRINT_LOW("Rxd SPS+PPS nPortIndex[%u] len[%u] data[%p]",
 (unsigned int)m_vendor_config.nPortIndex,
 (unsigned int)m_vendor_config.nDataSize,
                    m_vendor_config.pData);
 while (index < 2) {
 uint8 *psize;
                len = *pSrcBuf;
                len = len << 8;
                len |= *(pSrcBuf + 1);
                psize = (uint8 *) & len;
                memcpy(pDestBuf + nal_length, pSrcBuf + 2,len);
 for (unsigned int i = 0; i < nal_length; i++) {
                    pDestBuf[i] = psize[nal_length - 1 - i];
 }
                pDestBuf += len + nal_length;
                pSrcBuf += len + 2;
                index++;
                pSrcBuf++; // skip picture param set
                len = 0;
 }
 } else if (!strcmp(drv_ctx.kind, "OMX.qcom.video.decoder.mpeg4") ||
 !strcmp(drv_ctx.kind, "OMX.qcom.video.decoder.mpeg2")) {
            m_vendor_config.nPortIndex = config->nPortIndex;
            m_vendor_config.nDataSize = config->nDataSize;
            m_vendor_config.pData = (OMX_U8 *) malloc((config->nDataSize));
            memcpy(m_vendor_config.pData, config->pData,config->nDataSize);
 } else if (!strcmp(drv_ctx.kind, "OMX.qcom.video.decoder.vc1")) {
 if (m_vendor_config.pData) {
                free(m_vendor_config.pData);
                m_vendor_config.pData = NULL;
                m_vendor_config.nDataSize = 0;
 }

 if (((*((OMX_U32 *) config->pData)) &
                        VC1_SP_MP_START_CODE_MASK) ==
                    VC1_SP_MP_START_CODE) {
                DEBUG_PRINT_LOW("set_config - VC1 simple/main profile");
                m_vendor_config.nPortIndex = config->nPortIndex;
                m_vendor_config.nDataSize = config->nDataSize;
                m_vendor_config.pData =
 (OMX_U8 *) malloc(config->nDataSize);
                memcpy(m_vendor_config.pData, config->pData,
                        config->nDataSize);
                m_vc1_profile = VC1_SP_MP_RCV;
 } else if (*((OMX_U32 *) config->pData) == VC1_AP_SEQ_START_CODE) {
                DEBUG_PRINT_LOW("set_config - VC1 Advance profile");
                m_vendor_config.nPortIndex = config->nPortIndex;
                m_vendor_config.nDataSize = config->nDataSize;
                m_vendor_config.pData =
 (OMX_U8 *) malloc((config->nDataSize));
                memcpy(m_vendor_config.pData, config->pData,
                        config->nDataSize);
                m_vc1_profile = VC1_AP;
 } else if ((config->nDataSize == VC1_STRUCT_C_LEN)) {
                DEBUG_PRINT_LOW("set_config - VC1 Simple/Main profile struct C only");
                m_vendor_config.nPortIndex = config->nPortIndex;
                m_vendor_config.nDataSize  = config->nDataSize;
                m_vendor_config.pData = (OMX_U8*)malloc(config->nDataSize);
                memcpy(m_vendor_config.pData,config->pData,config->nDataSize);
                m_vc1_profile = VC1_SP_MP_RCV;
 } else {
                DEBUG_PRINT_LOW("set_config - Error: Unknown VC1 profile");
 }
 }
 return ret;
 } else if (configIndex == OMX_IndexConfigVideoNalSize) {

         struct v4l2_control temp;
         temp.id = V4L2_CID_MPEG_VIDC_VIDEO_STREAM_FORMAT;
 
         pNal = reinterpret_cast < OMX_VIDEO_CONFIG_NALSIZE * >(configData);
         switch (pNal->nNaluBytes) {
             case 0:
                temp.value = V4L2_MPEG_VIDC_VIDEO_NAL_FORMAT_STARTCODES;
 break;
 case 2:
                temp.value = V4L2_MPEG_VIDC_VIDEO_NAL_FORMAT_TWO_BYTE_LENGTH;
 break;
 case 4:
                temp.value = V4L2_MPEG_VIDC_VIDEO_NAL_FORMAT_FOUR_BYTE_LENGTH;
 break;
 default:
 return OMX_ErrorUnsupportedSetting;
 }

 if (!arbitrary_bytes) {
 /* In arbitrary bytes mode, the assembler strips out nal size and replaces
             * with start code, so only need to notify driver in frame by frame mode */
 if (ioctl(drv_ctx.video_driver_fd, VIDIOC_S_CTRL, &temp)) {
                DEBUG_PRINT_ERROR("Failed to set V4L2_CID_MPEG_VIDC_VIDEO_STREAM_FORMAT");
 return OMX_ErrorHardware;
 }
 }

        nal_length = pNal->nNaluBytes;
        m_frame_parser.init_nal_length(nal_length);

        DEBUG_PRINT_LOW("OMX_IndexConfigVideoNalSize called with Size %d", nal_length);
 return ret;
 } else if ((int)configIndex == (int)OMX_IndexVendorVideoFrameRate) {
        OMX_VENDOR_VIDEOFRAMERATE *config = (OMX_VENDOR_VIDEOFRAMERATE *) configData;
        DEBUG_PRINT_HIGH("Index OMX_IndexVendorVideoFrameRate %u", (unsigned int)config->nFps);

 if (config->nPortIndex == OMX_CORE_INPUT_PORT_INDEX) {
 if (config->bEnabled) {
 if ((config->nFps >> 16) > 0) {
                    DEBUG_PRINT_HIGH("set_config: frame rate set by omx client : %u",
 (unsigned int)config->nFps >> 16);
                    Q16ToFraction(config->nFps, drv_ctx.frame_rate.fps_numerator,
                            drv_ctx.frame_rate.fps_denominator);

 if (!drv_ctx.frame_rate.fps_numerator) {
                        DEBUG_PRINT_ERROR("Numerator is zero setting to 30");
                        drv_ctx.frame_rate.fps_numerator = 30;
 }

 if (drv_ctx.frame_rate.fps_denominator) {
                        drv_ctx.frame_rate.fps_numerator = (int)
                            drv_ctx.frame_rate.fps_numerator / drv_ctx.frame_rate.fps_denominator;
 }

                    drv_ctx.frame_rate.fps_denominator = 1;
                    frm_int = drv_ctx.frame_rate.fps_denominator * 1e6 /
                        drv_ctx.frame_rate.fps_numerator;

 struct v4l2_outputparm oparm;
 /*XXX: we're providing timing info as seconds per frame rather than frames
                     * per second.*/
                    oparm.timeperframe.numerator = drv_ctx.frame_rate.fps_denominator;
                    oparm.timeperframe.denominator = drv_ctx.frame_rate.fps_numerator;

 struct v4l2_streamparm sparm;
                    sparm.type = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;
                    sparm.parm.output = oparm;
 if (ioctl(drv_ctx.video_driver_fd, VIDIOC_S_PARM, &sparm)) {
                        DEBUG_PRINT_ERROR("Unable to convey fps info to driver, \
                                performance might be affected");
                        ret = OMX_ErrorHardware;
 }
                    client_set_fps = true;
 } else {
                    DEBUG_PRINT_ERROR("Frame rate not supported.");
                    ret = OMX_ErrorUnsupportedSetting;
 }
 } else {
                DEBUG_PRINT_HIGH("set_config: Disabled client's frame rate");
                client_set_fps = false;
 }
 } else {
            DEBUG_PRINT_ERROR(" Set_config: Bad Port idx %d",
 (int)config->nPortIndex);
            ret = OMX_ErrorBadPortIndex;
 }

 return ret;
 } else if ((int)configIndex == (int)OMX_QcomIndexConfigPerfLevel) {
        OMX_QCOM_VIDEO_CONFIG_PERF_LEVEL *perf =
 (OMX_QCOM_VIDEO_CONFIG_PERF_LEVEL *)configData;
 struct v4l2_control control;

        DEBUG_PRINT_LOW("Set perf level: %d", perf->ePerfLevel);

        control.id = V4L2_CID_MPEG_VIDC_SET_PERF_LEVEL;

 switch (perf->ePerfLevel) {
 case OMX_QCOM_PerfLevelNominal:
                control.value = V4L2_CID_MPEG_VIDC_PERF_LEVEL_NOMINAL;
 break;
 case OMX_QCOM_PerfLevelTurbo:
                control.value = V4L2_CID_MPEG_VIDC_PERF_LEVEL_TURBO;
 break;
 default:
                ret = OMX_ErrorUnsupportedSetting;
 break;
 }

 if (ret == OMX_ErrorNone) {
            ret = (ioctl(drv_ctx.video_driver_fd, VIDIOC_S_CTRL, &control) < 0) ?
                OMX_ErrorUnsupportedSetting : OMX_ErrorNone;
 }

 return ret;
 } else if ((int)configIndex == (int)OMX_IndexConfigPriority) {
        OMX_PARAM_U32TYPE *priority = (OMX_PARAM_U32TYPE *)configData;
        DEBUG_PRINT_LOW("Set_config: priority %d", priority->nU32);

 struct v4l2_control control;

        control.id = V4L2_CID_MPEG_VIDC_VIDEO_PRIORITY;
 if (priority->nU32 == 0)
            control.value = V4L2_MPEG_VIDC_VIDEO_PRIORITY_REALTIME_ENABLE;
 else
            control.value = V4L2_MPEG_VIDC_VIDEO_PRIORITY_REALTIME_DISABLE;

 if (ioctl(drv_ctx.video_driver_fd, VIDIOC_S_CTRL, &control)) {
            DEBUG_PRINT_ERROR("Failed to set Priority");
            ret = OMX_ErrorUnsupportedSetting;
 }
 return ret;
 } else if ((int)configIndex == (int)OMX_IndexConfigOperatingRate) {
        OMX_PARAM_U32TYPE *rate = (OMX_PARAM_U32TYPE *)configData;
        DEBUG_PRINT_LOW("Set_config: operating-rate %u fps", rate->nU32 >> 16);

 struct v4l2_control control;

        control.id = V4L2_CID_MPEG_VIDC_VIDEO_OPERATING_RATE;
        control.value = rate->nU32;

 if (ioctl(drv_ctx.video_driver_fd, VIDIOC_S_CTRL, &control)) {
            ret = errno == -EBUSY ? OMX_ErrorInsufficientResources :
                    OMX_ErrorUnsupportedSetting;
            DEBUG_PRINT_ERROR("Failed to set operating rate %u fps (%s)",
                    rate->nU32 >> 16, errno == -EBUSY ? "HW Overload" : strerror(errno));
 }
 return ret;
 }

 return OMX_ErrorNotImplemented;
}
