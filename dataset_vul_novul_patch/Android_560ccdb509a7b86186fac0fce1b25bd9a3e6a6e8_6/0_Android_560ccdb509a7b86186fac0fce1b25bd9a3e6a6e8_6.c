OMX_ERRORTYPE  omx_venc::set_config(OMX_IN OMX_HANDLETYPE      hComp,
        OMX_IN OMX_INDEXTYPE configIndex,
        OMX_IN OMX_PTR        configData)
{
 (void)hComp;
 if (configData == NULL) {
        DEBUG_PRINT_ERROR("ERROR: param is null");
 return OMX_ErrorBadParameter;
 }

 if (m_state == OMX_StateInvalid) {
        DEBUG_PRINT_ERROR("ERROR: config called in Invalid state");
 return OMX_ErrorIncorrectStateOperation;
 }


     switch ((int)configIndex) {
         case OMX_IndexConfigVideoBitrate:
             {
                VALIDATE_OMX_PARAM_DATA(configData, OMX_VIDEO_CONFIG_BITRATETYPE);
                 OMX_VIDEO_CONFIG_BITRATETYPE* pParam =
                     reinterpret_cast<OMX_VIDEO_CONFIG_BITRATETYPE*>(configData);
                 DEBUG_PRINT_HIGH("set_config(): OMX_IndexConfigVideoBitrate (%u)", (unsigned int)pParam->nEncodeBitrate);

 if (pParam->nPortIndex == PORT_INDEX_OUT) {
 if (handle->venc_set_config(configData, OMX_IndexConfigVideoBitrate) != true) {
                        DEBUG_PRINT_ERROR("ERROR: Setting OMX_IndexConfigVideoBitrate failed");
 return OMX_ErrorUnsupportedSetting;
 }

                    m_sConfigBitrate.nEncodeBitrate = pParam->nEncodeBitrate;
                    m_sParamBitrate.nTargetBitrate = pParam->nEncodeBitrate;
                    m_sOutPortDef.format.video.nBitrate = pParam->nEncodeBitrate;
 } else {
                    DEBUG_PRINT_ERROR("ERROR: Unsupported port index: %u", (unsigned int)pParam->nPortIndex);
 return OMX_ErrorBadPortIndex;
 }
 break;

             }
         case OMX_IndexConfigVideoFramerate:
             {
                VALIDATE_OMX_PARAM_DATA(configData, OMX_CONFIG_FRAMERATETYPE);
                 OMX_CONFIG_FRAMERATETYPE* pParam =
                     reinterpret_cast<OMX_CONFIG_FRAMERATETYPE*>(configData);
                 DEBUG_PRINT_HIGH("set_config(): OMX_IndexConfigVideoFramerate (0x%x)", (unsigned int)pParam->xEncodeFramerate);

 if (pParam->nPortIndex == PORT_INDEX_OUT) {
 if (handle->venc_set_config(configData, OMX_IndexConfigVideoFramerate) != true) {
                        DEBUG_PRINT_ERROR("ERROR: Setting OMX_IndexConfigVideoFramerate failed");
 return OMX_ErrorUnsupportedSetting;
 }

                    m_sConfigFramerate.xEncodeFramerate = pParam->xEncodeFramerate;
                    m_sOutPortDef.format.video.xFramerate = pParam->xEncodeFramerate;
                    m_sOutPortFormat.xFramerate = pParam->xEncodeFramerate;
 } else {
                    DEBUG_PRINT_ERROR("ERROR: Unsupported port index: %u", (unsigned int)pParam->nPortIndex);
 return OMX_ErrorBadPortIndex;
 }

 break;

             }
         case QOMX_IndexConfigVideoIntraperiod:
             {
                VALIDATE_OMX_PARAM_DATA(configData, QOMX_VIDEO_INTRAPERIODTYPE);
                 QOMX_VIDEO_INTRAPERIODTYPE* pParam =
                     reinterpret_cast<QOMX_VIDEO_INTRAPERIODTYPE*>(configData);
 
                DEBUG_PRINT_HIGH("set_config(): QOMX_IndexConfigVideoIntraperiod");
 if (pParam->nPortIndex == PORT_INDEX_OUT) {
#ifdef MAX_RES_720P
 if (pParam->nBFrames > 0) {
                        DEBUG_PRINT_ERROR("B frames not supported");
 return OMX_ErrorUnsupportedSetting;
 }
#endif
                    DEBUG_PRINT_HIGH("Old: P/B frames = %u/%u, New: P/B frames = %u/%u",
 (unsigned int)m_sIntraperiod.nPFrames, (unsigned int)m_sIntraperiod.nBFrames,
 (unsigned int)pParam->nPFrames, (unsigned int)pParam->nBFrames);
 if (m_sIntraperiod.nBFrames != pParam->nBFrames) {
 if(hier_b_enabled && m_state == OMX_StateLoaded) {
                            DEBUG_PRINT_INFO("B-frames setting is supported if HierB is enabled");
 }
 else {
                        DEBUG_PRINT_HIGH("Dynamically changing B-frames not supported");
 return OMX_ErrorUnsupportedSetting;
 }
 }
 if (handle->venc_set_config(configData, (OMX_INDEXTYPE) QOMX_IndexConfigVideoIntraperiod) != true) {
                        DEBUG_PRINT_ERROR("ERROR: Setting QOMX_IndexConfigVideoIntraperiod failed");
 return OMX_ErrorUnsupportedSetting;
 }
                    m_sIntraperiod.nPFrames = pParam->nPFrames;
                    m_sIntraperiod.nBFrames = pParam->nBFrames;
                    m_sIntraperiod.nIDRPeriod = pParam->nIDRPeriod;

 if (m_sOutPortFormat.eCompressionFormat == OMX_VIDEO_CodingMPEG4) {
                        m_sParamMPEG4.nPFrames = pParam->nPFrames;
 if (m_sParamMPEG4.eProfile != OMX_VIDEO_MPEG4ProfileSimple)
                            m_sParamMPEG4.nBFrames = pParam->nBFrames;
 else
                            m_sParamMPEG4.nBFrames = 0;
 } else if (m_sOutPortFormat.eCompressionFormat == OMX_VIDEO_CodingH263) {
                        m_sParamH263.nPFrames = pParam->nPFrames;
 } else {
                        m_sParamAVC.nPFrames = pParam->nPFrames;
 if ((m_sParamAVC.eProfile != OMX_VIDEO_AVCProfileBaseline) &&
 (m_sParamAVC.eProfile != (OMX_VIDEO_AVCPROFILETYPE) QOMX_VIDEO_AVCProfileConstrainedBaseline))
                            m_sParamAVC.nBFrames = pParam->nBFrames;
 else
                            m_sParamAVC.nBFrames = 0;
 }
 } else {
                    DEBUG_PRINT_ERROR("ERROR: (QOMX_IndexConfigVideoIntraperiod) Unsupported port index: %u", (unsigned int)pParam->nPortIndex);
 return OMX_ErrorBadPortIndex;
 }

 break;
 }

 
         case OMX_IndexConfigVideoIntraVOPRefresh:
             {
                VALIDATE_OMX_PARAM_DATA(configData, OMX_CONFIG_INTRAREFRESHVOPTYPE);
                 OMX_CONFIG_INTRAREFRESHVOPTYPE* pParam =
                     reinterpret_cast<OMX_CONFIG_INTRAREFRESHVOPTYPE*>(configData);
 
                DEBUG_PRINT_HIGH("set_config(): OMX_IndexConfigVideoIntraVOPRefresh");
 if (pParam->nPortIndex == PORT_INDEX_OUT) {
 if (handle->venc_set_config(configData,
                                OMX_IndexConfigVideoIntraVOPRefresh) != true) {
                        DEBUG_PRINT_ERROR("ERROR: Setting OMX_IndexConfigVideoIntraVOPRefresh failed");
 return OMX_ErrorUnsupportedSetting;
 }

                    m_sConfigIntraRefreshVOP.IntraRefreshVOP = pParam->IntraRefreshVOP;
 } else {
                    DEBUG_PRINT_ERROR("ERROR: Unsupported port index: %u", (unsigned int)pParam->nPortIndex);
 return OMX_ErrorBadPortIndex;
 }

 break;

             }
         case OMX_IndexConfigCommonRotate:
             {
                VALIDATE_OMX_PARAM_DATA(configData, OMX_CONFIG_ROTATIONTYPE);
                 OMX_CONFIG_ROTATIONTYPE *pParam =
                     reinterpret_cast<OMX_CONFIG_ROTATIONTYPE*>(configData);
                 OMX_S32 nRotation;

 if (pParam->nPortIndex != PORT_INDEX_OUT) {
                    DEBUG_PRINT_ERROR("ERROR: Unsupported port index: %u", (unsigned int)pParam->nPortIndex);
 return OMX_ErrorBadPortIndex;
 }
 if ( pParam->nRotation == 0 ||
                        pParam->nRotation == 90 ||
                        pParam->nRotation == 180 ||
                        pParam->nRotation == 270 ) {
                    DEBUG_PRINT_HIGH("set_config: Rotation Angle %u", (unsigned int)pParam->nRotation);
 } else {
                    DEBUG_PRINT_ERROR("ERROR: un supported Rotation %u", (unsigned int)pParam->nRotation);
 return OMX_ErrorUnsupportedSetting;
 }
                nRotation = pParam->nRotation - m_sConfigFrameRotation.nRotation;
 if (nRotation < 0)
                    nRotation = -nRotation;
 if (nRotation == 90 || nRotation == 270) {
                    DEBUG_PRINT_HIGH("set_config: updating device Dims");
 if (handle->venc_set_config(configData,
                                OMX_IndexConfigCommonRotate) != true) {
                        DEBUG_PRINT_ERROR("ERROR: Set OMX_IndexConfigCommonRotate failed");
 return OMX_ErrorUnsupportedSetting;
 } else {
                        OMX_U32 nFrameWidth;
                        OMX_U32 nFrameHeight;

                        DEBUG_PRINT_HIGH("set_config: updating port Dims");

                        nFrameWidth = m_sOutPortDef.format.video.nFrameWidth;
                        nFrameHeight = m_sOutPortDef.format.video.nFrameHeight;
                        m_sOutPortDef.format.video.nFrameWidth  = nFrameHeight;
                        m_sOutPortDef.format.video.nFrameHeight = nFrameWidth;
                        m_sConfigFrameRotation.nRotation = pParam->nRotation;
 }
 } else {
                    m_sConfigFrameRotation.nRotation = pParam->nRotation;
 }
 break;
 }
 case OMX_QcomIndexConfigVideoFramePackingArrangement:

             {
                 DEBUG_PRINT_HIGH("set_config(): OMX_QcomIndexConfigVideoFramePackingArrangement");
                 if (m_sOutPortFormat.eCompressionFormat == OMX_VIDEO_CodingAVC) {
                    VALIDATE_OMX_PARAM_DATA(configData, OMX_QCOM_FRAME_PACK_ARRANGEMENT);
                     OMX_QCOM_FRAME_PACK_ARRANGEMENT *configFmt =
                         (OMX_QCOM_FRAME_PACK_ARRANGEMENT *) configData;
                     extra_data_handle.set_frame_pack_data(configFmt);
 } else {
                    DEBUG_PRINT_ERROR("ERROR: FramePackingData not supported for non AVC compression");
 }
 break;

             }
         case QOMX_IndexConfigVideoLTRPeriod:
             {
                VALIDATE_OMX_PARAM_DATA(configData, QOMX_VIDEO_CONFIG_LTRPERIOD_TYPE);
                 QOMX_VIDEO_CONFIG_LTRPERIOD_TYPE* pParam = (QOMX_VIDEO_CONFIG_LTRPERIOD_TYPE*)configData;
                 if (!handle->venc_set_config(configData, (OMX_INDEXTYPE)QOMX_IndexConfigVideoLTRPeriod)) {
                     DEBUG_PRINT_ERROR("ERROR: Setting LTR period failed");
 return OMX_ErrorUnsupportedSetting;
 }
                memcpy(&m_sConfigLTRPeriod, pParam, sizeof(m_sConfigLTRPeriod));
 break;
 }

 
        case OMX_IndexConfigVideoVp8ReferenceFrame:
            {
                VALIDATE_OMX_PARAM_DATA(configData, OMX_VIDEO_VP8REFERENCEFRAMETYPE);
                OMX_VIDEO_VP8REFERENCEFRAMETYPE* pParam = (OMX_VIDEO_VP8REFERENCEFRAMETYPE*) configData;
                if (!handle->venc_set_config(pParam, (OMX_INDEXTYPE) OMX_IndexConfigVideoVp8ReferenceFrame)) {
                    DEBUG_PRINT_ERROR("ERROR: Setting VP8 reference frame");
 return OMX_ErrorUnsupportedSetting;
 }
               memcpy(&m_sConfigVp8ReferenceFrame, pParam, sizeof(m_sConfigVp8ReferenceFrame));
 break;
 }

 
         case QOMX_IndexConfigVideoLTRUse:
             {
                VALIDATE_OMX_PARAM_DATA(configData, QOMX_VIDEO_CONFIG_LTRUSE_TYPE);
                 QOMX_VIDEO_CONFIG_LTRUSE_TYPE* pParam = (QOMX_VIDEO_CONFIG_LTRUSE_TYPE*)configData;
                 if (!handle->venc_set_config(pParam, (OMX_INDEXTYPE)QOMX_IndexConfigVideoLTRUse)) {
                     DEBUG_PRINT_ERROR("ERROR: Setting LTR use failed");
 return OMX_ErrorUnsupportedSetting;
 }
                memcpy(&m_sConfigLTRUse, pParam, sizeof(m_sConfigLTRUse));
 break;

             }
         case QOMX_IndexConfigVideoLTRMark:
             {
                VALIDATE_OMX_PARAM_DATA(configData, QOMX_VIDEO_CONFIG_LTRMARK_TYPE);
                 QOMX_VIDEO_CONFIG_LTRMARK_TYPE* pParam = (QOMX_VIDEO_CONFIG_LTRMARK_TYPE*)configData;
                 if (!handle->venc_set_config(pParam, (OMX_INDEXTYPE)QOMX_IndexConfigVideoLTRMark)) {
                     DEBUG_PRINT_ERROR("ERROR: Setting LTR mark failed");
 return OMX_ErrorUnsupportedSetting;
 }
 break;

             }
         case OMX_IndexConfigVideoAVCIntraPeriod:
             {
                VALIDATE_OMX_PARAM_DATA(configData, OMX_VIDEO_CONFIG_AVCINTRAPERIOD);
                 OMX_VIDEO_CONFIG_AVCINTRAPERIOD *pParam = (OMX_VIDEO_CONFIG_AVCINTRAPERIOD*) configData;
                 DEBUG_PRINT_LOW("set_config: OMX_IndexConfigVideoAVCIntraPeriod");
                 if (!handle->venc_set_config(pParam, (OMX_INDEXTYPE)OMX_IndexConfigVideoAVCIntraPeriod)) {
                    DEBUG_PRINT_ERROR("ERROR: Setting OMX_IndexConfigVideoAVCIntraPeriod failed");
 return OMX_ErrorUnsupportedSetting;
 }
                memcpy(&m_sConfigAVCIDRPeriod, pParam, sizeof(m_sConfigAVCIDRPeriod));
 break;

             }
         case OMX_IndexConfigCommonDeinterlace:
             {
                VALIDATE_OMX_PARAM_DATA(configData, OMX_VIDEO_CONFIG_DEINTERLACE);
                 OMX_VIDEO_CONFIG_DEINTERLACE *pParam = (OMX_VIDEO_CONFIG_DEINTERLACE*) configData;
                 DEBUG_PRINT_LOW("set_config: OMX_IndexConfigCommonDeinterlace");
                 if (!handle->venc_set_config(pParam, (OMX_INDEXTYPE)OMX_IndexConfigCommonDeinterlace)) {
                    DEBUG_PRINT_ERROR("ERROR: Setting OMX_IndexConfigCommonDeinterlace failed");
 return OMX_ErrorUnsupportedSetting;
 }
                memcpy(&m_sConfigDeinterlace, pParam, sizeof(m_sConfigDeinterlace));
 break;

             }
         case OMX_QcomIndexConfigVideoVencPerfMode:
             {
                VALIDATE_OMX_PARAM_DATA(configData, QOMX_EXTNINDEX_VIDEO_PERFMODE);
                 QOMX_EXTNINDEX_VIDEO_PERFMODE* pParam = (QOMX_EXTNINDEX_VIDEO_PERFMODE*)configData;
                 if (!handle->venc_set_config(pParam, (OMX_INDEXTYPE)OMX_QcomIndexConfigVideoVencPerfMode)) {
                     DEBUG_PRINT_ERROR("ERROR: Setting OMX_QcomIndexConfigVideoVencPerfMode failed");
 return OMX_ErrorUnsupportedSetting;
 }
 break;

             }
         case OMX_IndexConfigPriority:
             {
                VALIDATE_OMX_PARAM_DATA(configData, OMX_PARAM_U32TYPE);
                 if (!handle->venc_set_config(configData, (OMX_INDEXTYPE)OMX_IndexConfigPriority)) {
                     DEBUG_PRINT_ERROR("Failed to set OMX_IndexConfigPriority");
                     return OMX_ErrorUnsupportedSetting;
 }
 break;

             }
         case OMX_IndexConfigOperatingRate:
             {
                VALIDATE_OMX_PARAM_DATA(configData, OMX_PARAM_U32TYPE);
                 if (!handle->venc_set_config(configData, (OMX_INDEXTYPE)OMX_IndexConfigOperatingRate)) {
                     DEBUG_PRINT_ERROR("Failed to set OMX_IndexConfigOperatingRate");
                     return handle->hw_overload ? OMX_ErrorInsufficientResources :
                            OMX_ErrorUnsupportedSetting;
 }
 break;
 }
 default:
            DEBUG_PRINT_ERROR("ERROR: unsupported index %d", (int) configIndex);
 break;
 }

 return OMX_ErrorNone;
}
