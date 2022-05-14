OMX_ERRORTYPE  omx_video::get_config(OMX_IN OMX_HANDLETYPE      hComp,
        OMX_IN OMX_INDEXTYPE configIndex,
        OMX_INOUT OMX_PTR     configData)
{
 (void)hComp;

 if (configData == NULL) {
        DEBUG_PRINT_ERROR("ERROR: param is null");
 return OMX_ErrorBadParameter;
 }

 if (m_state == OMX_StateInvalid) {
        DEBUG_PRINT_ERROR("ERROR: can't be in invalid state");
 return OMX_ErrorIncorrectStateOperation;
 }


     switch ((int)configIndex) {
         case OMX_IndexConfigVideoBitrate:
             {
                 OMX_VIDEO_CONFIG_BITRATETYPE* pParam = reinterpret_cast<OMX_VIDEO_CONFIG_BITRATETYPE*>(configData);
                 memcpy(pParam, &m_sConfigBitrate, sizeof(m_sConfigBitrate));
                 break;
             }
         case OMX_IndexConfigVideoFramerate:
             {
                 OMX_CONFIG_FRAMERATETYPE* pParam = reinterpret_cast<OMX_CONFIG_FRAMERATETYPE*>(configData);
                 memcpy(pParam, &m_sConfigFramerate, sizeof(m_sConfigFramerate));
                 break;
             }
         case OMX_IndexConfigCommonRotate:
             {
                 OMX_CONFIG_ROTATIONTYPE* pParam = reinterpret_cast<OMX_CONFIG_ROTATIONTYPE*>(configData);
                 memcpy(pParam, &m_sConfigFrameRotation, sizeof(m_sConfigFrameRotation));
                 break;
 }

         case QOMX_IndexConfigVideoIntraperiod:
             {
                 DEBUG_PRINT_LOW("get_config:QOMX_IndexConfigVideoIntraperiod");
                 QOMX_VIDEO_INTRAPERIODTYPE* pParam = reinterpret_cast<QOMX_VIDEO_INTRAPERIODTYPE*>(configData);
                 memcpy(pParam, &m_sIntraperiod, sizeof(m_sIntraperiod));
                 break;
             }
         case OMX_IndexConfigVideoAVCIntraPeriod:
             {
                 OMX_VIDEO_CONFIG_AVCINTRAPERIOD *pParam =
                     reinterpret_cast<OMX_VIDEO_CONFIG_AVCINTRAPERIOD*>(configData);
                 DEBUG_PRINT_LOW("get_config: OMX_IndexConfigVideoAVCIntraPeriod");
                memcpy(pParam, &m_sConfigAVCIDRPeriod, sizeof(m_sConfigAVCIDRPeriod));
 break;

             }
         case OMX_IndexConfigCommonDeinterlace:
             {
                 OMX_VIDEO_CONFIG_DEINTERLACE *pParam =
                     reinterpret_cast<OMX_VIDEO_CONFIG_DEINTERLACE*>(configData);
                 DEBUG_PRINT_LOW("get_config: OMX_IndexConfigCommonDeinterlace");
                memcpy(pParam, &m_sConfigDeinterlace, sizeof(m_sConfigDeinterlace));
 break;

             }
        case OMX_IndexConfigVideoVp8ReferenceFrame:
            {
                OMX_VIDEO_VP8REFERENCEFRAMETYPE* pParam =
                    reinterpret_cast<OMX_VIDEO_VP8REFERENCEFRAMETYPE*>(configData);
                DEBUG_PRINT_LOW("get_config: OMX_IndexConfigVideoVp8ReferenceFrame");
               memcpy(pParam, &m_sConfigVp8ReferenceFrame, sizeof(m_sConfigVp8ReferenceFrame));
 break;

            }
         case OMX_QcomIndexConfigPerfLevel:
             {
                 OMX_U32 perflevel;
                 OMX_QCOM_VIDEO_CONFIG_PERF_LEVEL *pParam =
                     reinterpret_cast<OMX_QCOM_VIDEO_CONFIG_PERF_LEVEL*>(configData);
                DEBUG_PRINT_LOW("get_config: OMX_QcomIndexConfigPerfLevel");
 if (!dev_get_performance_level(&perflevel)) {
                    DEBUG_PRINT_ERROR("Invalid entry returned from get_performance_level %d",
                        pParam->ePerfLevel);
 } else {
                    pParam->ePerfLevel = (QOMX_VIDEO_PERF_LEVEL)perflevel;
 }
 break;
 }
 default:
            DEBUG_PRINT_ERROR("ERROR: unsupported index %d", (int) configIndex);
 return OMX_ErrorUnsupportedIndex;
 }
 return OMX_ErrorNone;

}
