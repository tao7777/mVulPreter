OMX_ERRORTYPE  omx_video::get_parameter(OMX_IN OMX_HANDLETYPE     hComp,
        OMX_IN OMX_INDEXTYPE paramIndex,
        OMX_INOUT OMX_PTR     paramData)
{
 (void)hComp;
    OMX_ERRORTYPE eRet = OMX_ErrorNone;
 unsigned int height=0,width = 0;

    DEBUG_PRINT_LOW("get_parameter:");
 if (m_state == OMX_StateInvalid) {
        DEBUG_PRINT_ERROR("ERROR: Get Param in Invalid State");
 return OMX_ErrorInvalidState;
 }
 if (paramData == NULL) {
        DEBUG_PRINT_ERROR("ERROR: Get Param in Invalid paramData");
 return OMX_ErrorBadParameter;
 }

     switch ((int)paramIndex) {
         case OMX_IndexParamPortDefinition:
             {
                VALIDATE_OMX_PARAM_DATA(paramData, OMX_PARAM_PORTDEFINITIONTYPE);
                 OMX_PARAM_PORTDEFINITIONTYPE *portDefn;
                 portDefn = (OMX_PARAM_PORTDEFINITIONTYPE *) paramData;
 
                DEBUG_PRINT_LOW("get_parameter: OMX_IndexParamPortDefinition");
 if (portDefn->nPortIndex == (OMX_U32) PORT_INDEX_IN) {
                    dev_get_buf_req (&m_sInPortDef.nBufferCountMin,
 &m_sInPortDef.nBufferCountActual,
 &m_sInPortDef.nBufferSize,
                        m_sInPortDef.nPortIndex);
                    DEBUG_PRINT_LOW("m_sInPortDef: size = %u, min cnt = %u, actual cnt = %u",
 (unsigned int)m_sInPortDef.nBufferSize, (unsigned int)m_sInPortDef.nBufferCountMin,
 (unsigned int)m_sInPortDef.nBufferCountActual);
                    memcpy(portDefn, &m_sInPortDef, sizeof(m_sInPortDef));
#ifdef _ANDROID_ICS_
 if (meta_mode_enable) {
                        portDefn->nBufferSize = sizeof(encoder_media_buffer_type);
 }
 if (mUseProxyColorFormat) {
                        portDefn->format.video.eColorFormat =
 (OMX_COLOR_FORMATTYPE)QOMX_COLOR_FormatAndroidOpaque;
 }
#endif
 } else if (portDefn->nPortIndex == (OMX_U32) PORT_INDEX_OUT) {
 if (m_state != OMX_StateExecuting) {
                    dev_get_buf_req (&m_sOutPortDef.nBufferCountMin,
 &m_sOutPortDef.nBufferCountActual,
 &m_sOutPortDef.nBufferSize,
                            m_sOutPortDef.nPortIndex);
 }
                    DEBUG_PRINT_LOW("m_sOutPortDef: size = %u, min cnt = %u, actual cnt = %u",
 (unsigned int)m_sOutPortDef.nBufferSize, (unsigned int)m_sOutPortDef.nBufferCountMin,
 (unsigned int)m_sOutPortDef.nBufferCountActual);
                    memcpy(portDefn, &m_sOutPortDef, sizeof(m_sOutPortDef));
 } else {
                    DEBUG_PRINT_ERROR("ERROR: GetParameter called on Bad Port Index");
                    eRet = OMX_ErrorBadPortIndex;
 }
 break;

             }
         case OMX_IndexParamVideoInit:
             {
                VALIDATE_OMX_PARAM_DATA(paramData, OMX_PORT_PARAM_TYPE);
                 OMX_PORT_PARAM_TYPE *portParamType =
                     (OMX_PORT_PARAM_TYPE *) paramData;
                 DEBUG_PRINT_LOW("get_parameter: OMX_IndexParamVideoInit");

                memcpy(portParamType, &m_sPortParam, sizeof(m_sPortParam));
 break;

             }
         case OMX_IndexParamVideoPortFormat:
             {
                VALIDATE_OMX_PARAM_DATA(paramData, OMX_VIDEO_PARAM_PORTFORMATTYPE);
                 OMX_VIDEO_PARAM_PORTFORMATTYPE *portFmt =
                     (OMX_VIDEO_PARAM_PORTFORMATTYPE *)paramData;
                 DEBUG_PRINT_LOW("get_parameter: OMX_IndexParamVideoPortFormat");

 if (portFmt->nPortIndex == (OMX_U32) PORT_INDEX_IN) {
 unsigned index = portFmt->nIndex;
 int supportedFormats[] = {
 [0] = QOMX_COLOR_FORMATYUV420PackedSemiPlanar32m,
 [1] = QOMX_COLOR_FormatAndroidOpaque,
 [2] = OMX_COLOR_FormatYUV420SemiPlanar,
 };

 if (index > (sizeof(supportedFormats)/sizeof(*supportedFormats) - 1))
                        eRet = OMX_ErrorNoMore;
 else {
                        memcpy(portFmt, &m_sInPortFormat, sizeof(m_sInPortFormat));
                        portFmt->nIndex = index; //restore index set from client
                        portFmt->eColorFormat = (OMX_COLOR_FORMATTYPE)supportedFormats[index];
 }
 } else if (portFmt->nPortIndex == (OMX_U32) PORT_INDEX_OUT) {
                    memcpy(portFmt, &m_sOutPortFormat, sizeof(m_sOutPortFormat));
 } else {
                    DEBUG_PRINT_ERROR("ERROR: GetParameter called on Bad Port Index");
                    eRet = OMX_ErrorBadPortIndex;
 }
 break;

             }
         case OMX_IndexParamVideoBitrate:
             {
                VALIDATE_OMX_PARAM_DATA(paramData, OMX_VIDEO_PARAM_BITRATETYPE);
                 OMX_VIDEO_PARAM_BITRATETYPE* pParam = (OMX_VIDEO_PARAM_BITRATETYPE*)paramData;
                 DEBUG_PRINT_LOW("get_parameter: OMX_IndexParamVideoBitrate");
 
 if (pParam->nPortIndex == (OMX_U32) PORT_INDEX_OUT) {
                    memcpy(pParam, &m_sParamBitrate, sizeof(m_sParamBitrate));
 } else {
                    DEBUG_PRINT_ERROR("ERROR: GetParameter called on Bad Port Index");
                    eRet = OMX_ErrorBadPortIndex;
 }

 break;

             }
         case OMX_IndexParamVideoMpeg4:
             {
                VALIDATE_OMX_PARAM_DATA(paramData, OMX_VIDEO_PARAM_MPEG4TYPE);
                 OMX_VIDEO_PARAM_MPEG4TYPE* pParam = (OMX_VIDEO_PARAM_MPEG4TYPE*)paramData;
                 DEBUG_PRINT_LOW("get_parameter: OMX_IndexParamVideoMpeg4");
                 memcpy(pParam, &m_sParamMPEG4, sizeof(m_sParamMPEG4));
 break;

             }
         case OMX_IndexParamVideoH263:
             {
                VALIDATE_OMX_PARAM_DATA(paramData, OMX_VIDEO_PARAM_H263TYPE);
                 OMX_VIDEO_PARAM_H263TYPE* pParam = (OMX_VIDEO_PARAM_H263TYPE*)paramData;
                 DEBUG_PRINT_LOW("get_parameter: OMX_IndexParamVideoH263");
                 memcpy(pParam, &m_sParamH263, sizeof(m_sParamH263));
 break;

             }
         case OMX_IndexParamVideoAvc:
             {
                VALIDATE_OMX_PARAM_DATA(paramData, OMX_VIDEO_PARAM_AVCTYPE);
                 OMX_VIDEO_PARAM_AVCTYPE* pParam = (OMX_VIDEO_PARAM_AVCTYPE*)paramData;
                 DEBUG_PRINT_LOW("get_parameter: OMX_IndexParamVideoAvc");
                 memcpy(pParam, &m_sParamAVC, sizeof(m_sParamAVC));
 break;

             }
         case (OMX_INDEXTYPE)OMX_IndexParamVideoVp8:
             {
                VALIDATE_OMX_PARAM_DATA(paramData, OMX_VIDEO_PARAM_VP8TYPE);
                 OMX_VIDEO_PARAM_VP8TYPE* pParam = (OMX_VIDEO_PARAM_VP8TYPE*)paramData;
                 DEBUG_PRINT_LOW("get_parameter: OMX_IndexParamVideoVp8");
                 memcpy(pParam, &m_sParamVP8, sizeof(m_sParamVP8));
 break;

             }
         case (OMX_INDEXTYPE)OMX_IndexParamVideoHevc:
             {
                VALIDATE_OMX_PARAM_DATA(paramData, OMX_VIDEO_PARAM_HEVCTYPE);
                 OMX_VIDEO_PARAM_HEVCTYPE* pParam = (OMX_VIDEO_PARAM_HEVCTYPE*)paramData;
                 DEBUG_PRINT_LOW("get_parameter: OMX_IndexParamVideoHevc");
                 memcpy(pParam, &m_sParamHEVC, sizeof(m_sParamHEVC));
 break;

             }
         case OMX_IndexParamVideoProfileLevelQuerySupported:
             {
                VALIDATE_OMX_PARAM_DATA(paramData, OMX_VIDEO_PARAM_PROFILELEVELTYPE);
                 OMX_VIDEO_PARAM_PROFILELEVELTYPE* pParam = (OMX_VIDEO_PARAM_PROFILELEVELTYPE*)paramData;
                 DEBUG_PRINT_LOW("get_parameter: OMX_IndexParamVideoProfileLevelQuerySupported");
                 eRet = get_supported_profile_level(pParam);
 if (eRet && eRet != OMX_ErrorNoMore)
                    DEBUG_PRINT_ERROR("Invalid entry returned from get_supported_profile_level %u, %u",
 (unsigned int)pParam->eProfile, (unsigned int)pParam->eLevel);
 break;

             }
         case OMX_IndexParamVideoProfileLevelCurrent:
             {
                VALIDATE_OMX_PARAM_DATA(paramData, OMX_VIDEO_PARAM_PROFILELEVELTYPE);
                 OMX_VIDEO_PARAM_PROFILELEVELTYPE* pParam = (OMX_VIDEO_PARAM_PROFILELEVELTYPE*)paramData;
                 DEBUG_PRINT_LOW("get_parameter: OMX_IndexParamVideoProfileLevelCurrent");
                 memcpy(pParam, &m_sParamProfileLevel, sizeof(m_sParamProfileLevel));
 break;
 }

             /*Component should support this port definition*/
         case OMX_IndexParamAudioInit:
             {
                VALIDATE_OMX_PARAM_DATA(paramData, OMX_PORT_PARAM_TYPE);
                 OMX_PORT_PARAM_TYPE *audioPortParamType = (OMX_PORT_PARAM_TYPE *) paramData;
                 DEBUG_PRINT_LOW("get_parameter: OMX_IndexParamAudioInit");
                 memcpy(audioPortParamType, &m_sPortParam_audio, sizeof(m_sPortParam_audio));
 break;
 }

             /*Component should support this port definition*/
         case OMX_IndexParamImageInit:
             {
                VALIDATE_OMX_PARAM_DATA(paramData, OMX_PORT_PARAM_TYPE);
                 OMX_PORT_PARAM_TYPE *imagePortParamType = (OMX_PORT_PARAM_TYPE *) paramData;
                 DEBUG_PRINT_LOW("get_parameter: OMX_IndexParamImageInit");
                 memcpy(imagePortParamType, &m_sPortParam_img, sizeof(m_sPortParam_img));
 break;

 }
 /*Component should support this port definition*/
 case OMX_IndexParamOtherInit:
 {
                DEBUG_PRINT_ERROR("ERROR: get_parameter: OMX_IndexParamOtherInit %08x", paramIndex);
                eRet =OMX_ErrorUnsupportedIndex;
 break;

             }
         case OMX_IndexParamStandardComponentRole:
             {
                VALIDATE_OMX_PARAM_DATA(paramData, OMX_PARAM_COMPONENTROLETYPE);
                 OMX_PARAM_COMPONENTROLETYPE *comp_role;
                 comp_role = (OMX_PARAM_COMPONENTROLETYPE *) paramData;
                 comp_role->nVersion.nVersion = OMX_SPEC_VERSION;
                comp_role->nSize = sizeof(*comp_role);

                DEBUG_PRINT_LOW("Getparameter: OMX_IndexParamStandardComponentRole %d",paramIndex);
                strlcpy((char*)comp_role->cRole,(const char*)m_cRole,OMX_MAX_STRINGNAME_SIZE);
 break;
 }

             /* Added for parameter test */
         case OMX_IndexParamPriorityMgmt:
             {
                VALIDATE_OMX_PARAM_DATA(paramData, OMX_PRIORITYMGMTTYPE);
                 OMX_PRIORITYMGMTTYPE *priorityMgmType = (OMX_PRIORITYMGMTTYPE *) paramData;
                 DEBUG_PRINT_LOW("get_parameter: OMX_IndexParamPriorityMgmt");
                 memcpy(priorityMgmType, &m_sPriorityMgmt, sizeof(m_sPriorityMgmt));
 break;
 }

             /* Added for parameter test */
         case OMX_IndexParamCompBufferSupplier:
             {
                VALIDATE_OMX_PARAM_DATA(paramData, OMX_PARAM_BUFFERSUPPLIERTYPE);
                 OMX_PARAM_BUFFERSUPPLIERTYPE *bufferSupplierType = (OMX_PARAM_BUFFERSUPPLIERTYPE*) paramData;
                 DEBUG_PRINT_LOW("get_parameter: OMX_IndexParamCompBufferSupplier");
                 if (bufferSupplierType->nPortIndex ==(OMX_U32) PORT_INDEX_IN) {
                    memcpy(bufferSupplierType, &m_sInBufSupplier, sizeof(m_sInBufSupplier));
 } else if (bufferSupplierType->nPortIndex ==(OMX_U32) PORT_INDEX_OUT) {
                    memcpy(bufferSupplierType, &m_sOutBufSupplier, sizeof(m_sOutBufSupplier));
 } else {
                    DEBUG_PRINT_ERROR("ERROR: GetParameter called on Bad Port Index");
                    eRet = OMX_ErrorBadPortIndex;
 }
 break;
 }

 
         case OMX_IndexParamVideoQuantization:
             {
                VALIDATE_OMX_PARAM_DATA(paramData, OMX_VIDEO_PARAM_QUANTIZATIONTYPE);
                 OMX_VIDEO_PARAM_QUANTIZATIONTYPE *session_qp = (OMX_VIDEO_PARAM_QUANTIZATIONTYPE*) paramData;
                 DEBUG_PRINT_LOW("get_parameter: OMX_IndexParamVideoQuantization");
                 memcpy(session_qp, &m_sSessionQuantization, sizeof(m_sSessionQuantization));
 break;
 }

 
         case OMX_QcomIndexParamVideoQPRange:
             {
                VALIDATE_OMX_PARAM_DATA(paramData, OMX_QCOM_VIDEO_PARAM_QPRANGETYPE);
                 OMX_QCOM_VIDEO_PARAM_QPRANGETYPE *qp_range = (OMX_QCOM_VIDEO_PARAM_QPRANGETYPE*) paramData;
                 DEBUG_PRINT_LOW("get_parameter: OMX_QcomIndexParamVideoQPRange");
                 memcpy(qp_range, &m_sSessionQPRange, sizeof(m_sSessionQPRange));
 break;
 }

 
         case OMX_IndexParamVideoErrorCorrection:
             {
                VALIDATE_OMX_PARAM_DATA(paramData, OMX_VIDEO_PARAM_ERRORCORRECTIONTYPE);
                 OMX_VIDEO_PARAM_ERRORCORRECTIONTYPE* errorresilience = (OMX_VIDEO_PARAM_ERRORCORRECTIONTYPE*)paramData;
                 DEBUG_PRINT_LOW("OMX_IndexParamVideoErrorCorrection");
                 errorresilience->bEnableHEC = m_sErrorCorrection.bEnableHEC;
                errorresilience->bEnableResync = m_sErrorCorrection.bEnableResync;
                errorresilience->nResynchMarkerSpacing = m_sErrorCorrection.nResynchMarkerSpacing;
 break;

             }
         case OMX_IndexParamVideoIntraRefresh:
             {
                VALIDATE_OMX_PARAM_DATA(paramData, OMX_VIDEO_PARAM_INTRAREFRESHTYPE);
                 OMX_VIDEO_PARAM_INTRAREFRESHTYPE* intrarefresh = (OMX_VIDEO_PARAM_INTRAREFRESHTYPE*)paramData;
                 DEBUG_PRINT_LOW("OMX_IndexParamVideoIntraRefresh");
                 DEBUG_PRINT_ERROR("OMX_IndexParamVideoIntraRefresh GET");
                intrarefresh->eRefreshMode = m_sIntraRefresh.eRefreshMode;
                intrarefresh->nCirMBs = m_sIntraRefresh.nCirMBs;
 break;
 }
 case OMX_QcomIndexPortDefn:

             break;
         case OMX_COMPONENT_CAPABILITY_TYPE_INDEX:
             {
                VALIDATE_OMX_PARAM_DATA(paramData, OMXComponentCapabilityFlagsType);
                 OMXComponentCapabilityFlagsType *pParam = reinterpret_cast<OMXComponentCapabilityFlagsType*>(paramData);
                 DEBUG_PRINT_LOW("get_parameter: OMX_COMPONENT_CAPABILITY_TYPE_INDEX");
                 pParam->iIsOMXComponentMultiThreaded = OMX_TRUE;
                pParam->iOMXComponentSupportsExternalOutputBufferAlloc = OMX_FALSE;
                pParam->iOMXComponentSupportsExternalInputBufferAlloc = OMX_TRUE;
                pParam->iOMXComponentSupportsMovableInputBuffers = OMX_TRUE;
                pParam->iOMXComponentUsesNALStartCodes = OMX_TRUE;
                pParam->iOMXComponentSupportsPartialFrames = OMX_FALSE;
                pParam->iOMXComponentCanHandleIncompleteFrames = OMX_FALSE;
                pParam->iOMXComponentUsesFullAVCFrames = OMX_FALSE;
                m_use_input_pmem = OMX_TRUE;
                DEBUG_PRINT_LOW("Supporting capability index in encoder node");
 break;
 }

 #if !defined(MAX_RES_720P) || defined(_MSM8974_)
         case OMX_QcomIndexParamIndexExtraDataType:
             {
                VALIDATE_OMX_PARAM_DATA(paramData, QOMX_INDEXEXTRADATATYPE);
                 DEBUG_PRINT_LOW("get_parameter: OMX_QcomIndexParamIndexExtraDataType");
                 QOMX_INDEXEXTRADATATYPE *pParam = (QOMX_INDEXEXTRADATATYPE *)paramData;
                 if (pParam->nIndex == (OMX_INDEXTYPE)OMX_ExtraDataVideoEncoderSliceInfo) {
 if (pParam->nPortIndex == PORT_INDEX_OUT) {
                        pParam->bEnabled =
 (OMX_BOOL)(m_sExtraData & VEN_EXTRADATA_SLICEINFO);
                        DEBUG_PRINT_HIGH("Slice Info extradata %d", pParam->bEnabled);
 } else {
                        DEBUG_PRINT_ERROR("get_parameter: slice information is "
 "valid for output port only");
                        eRet =OMX_ErrorUnsupportedIndex;
 }
 } else if (pParam->nIndex == (OMX_INDEXTYPE)OMX_ExtraDataVideoEncoderMBInfo) {
 if (pParam->nPortIndex == PORT_INDEX_OUT) {
                        pParam->bEnabled =
 (OMX_BOOL)(m_sExtraData & VEN_EXTRADATA_MBINFO);
                        DEBUG_PRINT_HIGH("MB Info extradata %d", pParam->bEnabled);
 } else {
                        DEBUG_PRINT_ERROR("get_parameter: MB information is "
 "valid for output port only");
                        eRet = OMX_ErrorUnsupportedIndex;
 }
 }
#ifndef _MSM8974_
 else if (pParam->nIndex == (OMX_INDEXTYPE)OMX_ExtraDataVideoLTRInfo) {
 if (pParam->nPortIndex == PORT_INDEX_OUT) {
                        pParam->bEnabled =
 (OMX_BOOL)(m_sExtraData & VEN_EXTRADATA_LTRINFO);
                        DEBUG_PRINT_HIGH("LTR Info extradata %d", pParam->bEnabled);
 } else {
                        DEBUG_PRINT_ERROR("get_parameter: LTR information is "
 "valid for output port only");
                        eRet = OMX_ErrorUnsupportedIndex;
 }
 }
#endif
 else {
                    DEBUG_PRINT_ERROR("get_parameter: unsupported extradata index (0x%x)",
                            pParam->nIndex);
                    eRet = OMX_ErrorUnsupportedIndex;
 }
 break;

             }
         case QOMX_IndexParamVideoLTRCountRangeSupported:
             {
                VALIDATE_OMX_PARAM_DATA(paramData, QOMX_EXTNINDEX_RANGETYPE);
                 DEBUG_PRINT_HIGH("get_parameter: QOMX_IndexParamVideoLTRCountRangeSupported");
                 QOMX_EXTNINDEX_RANGETYPE *pParam = (QOMX_EXTNINDEX_RANGETYPE *)paramData;
                 if (pParam->nPortIndex == PORT_INDEX_OUT) {
                    OMX_U32 min = 0, max = 0, step_size = 0;
 if (dev_get_capability_ltrcount(&min, &max, &step_size)) {
                        pParam->nMin = min;
                        pParam->nMax = max;
                        pParam->nStepSize = step_size;
 } else {
                        DEBUG_PRINT_ERROR("get_parameter: get_capability_ltrcount failed");
                        eRet = OMX_ErrorUndefined;
 }
 } else {
                    DEBUG_PRINT_ERROR("LTR count range is valid for output port only");
                    eRet = OMX_ErrorUnsupportedIndex;
 }
 }

             break;
         case OMX_QcomIndexParamVideoLTRCount:
             {
                VALIDATE_OMX_PARAM_DATA(paramData, OMX_QCOM_VIDEO_PARAM_LTRCOUNT_TYPE);
                 DEBUG_PRINT_LOW("get_parameter: OMX_QcomIndexParamVideoLTRCount");
                 OMX_QCOM_VIDEO_PARAM_LTRCOUNT_TYPE *pParam =
                         reinterpret_cast<OMX_QCOM_VIDEO_PARAM_LTRCOUNT_TYPE*>(paramData);
                memcpy(pParam, &m_sParamLTRCount, sizeof(m_sParamLTRCount));
 break;
 }

 #endif
         case QOMX_IndexParamVideoSyntaxHdr:
             {
                VALIDATE_OMX_PARAM_DATA(paramData, QOMX_EXTNINDEX_PARAMTYPE);
                 DEBUG_PRINT_HIGH("QOMX_IndexParamVideoSyntaxHdr");
                 QOMX_EXTNINDEX_PARAMTYPE* pParam =
                     reinterpret_cast<QOMX_EXTNINDEX_PARAMTYPE*>(paramData);
 if (pParam->pData == NULL) {
                    DEBUG_PRINT_ERROR("Error: Data buffer is NULL");
                    eRet = OMX_ErrorBadParameter;
 break;
 }
 if (get_syntaxhdr_enable == false) {
                    DEBUG_PRINT_ERROR("ERROR: get_parameter: Get syntax header disabled");
                    eRet = OMX_ErrorUnsupportedIndex;
 break;
 }
                BITMASK_SET(&m_flags, OMX_COMPONENT_LOADED_START_PENDING);
 if (dev_loaded_start()) {
                    DEBUG_PRINT_LOW("device start successful");
 } else {
                    DEBUG_PRINT_ERROR("device start failed");
                    BITMASK_CLEAR(&m_flags, OMX_COMPONENT_LOADED_START_PENDING);
 return OMX_ErrorHardware;
 }
 if (dev_get_seq_hdr(pParam->pData,
 (unsigned)(pParam->nSize - sizeof(QOMX_EXTNINDEX_PARAMTYPE)),
 (unsigned *)(void *)&pParam->nDataSize)) {
                    DEBUG_PRINT_HIGH("get syntax header successful (hdrlen = %u)",
 (unsigned int)pParam->nDataSize);
 for (unsigned i = 0; i < pParam->nDataSize; i++) {
                        DEBUG_PRINT_LOW("Header[%d] = %x", i, *((char *)pParam->pData + i));
 }
 } else {
                    DEBUG_PRINT_ERROR("Error returned from GetSyntaxHeader()");
                    eRet = OMX_ErrorHardware;
 }
                BITMASK_SET(&m_flags, OMX_COMPONENT_LOADED_STOP_PENDING);
 if (dev_loaded_stop()) {
                    DEBUG_PRINT_LOW("device stop successful");
 } else {
                    DEBUG_PRINT_ERROR("device stop failed");
                    BITMASK_CLEAR(&m_flags, OMX_COMPONENT_LOADED_STOP_PENDING);
                    eRet = OMX_ErrorHardware;
 }
 break;

             }
         case OMX_QcomIndexHierarchicalStructure:
             {
                VALIDATE_OMX_PARAM_DATA(paramData, QOMX_VIDEO_HIERARCHICALLAYERS);
                 QOMX_VIDEO_HIERARCHICALLAYERS* hierp = (QOMX_VIDEO_HIERARCHICALLAYERS*) paramData;
                 DEBUG_PRINT_LOW("get_parameter: OMX_QcomIndexHierarchicalStructure");
                 memcpy(hierp, &m_sHierLayers, sizeof(m_sHierLayers));
 break;

             }
         case OMX_QcomIndexParamPerfLevel:
             {
                VALIDATE_OMX_PARAM_DATA(paramData, OMX_QCOM_VIDEO_PARAM_PERF_LEVEL);
                 OMX_U32 perflevel;
                 OMX_QCOM_VIDEO_PARAM_PERF_LEVEL *pParam =
                     reinterpret_cast<OMX_QCOM_VIDEO_PARAM_PERF_LEVEL*>(paramData);
                DEBUG_PRINT_LOW("get_parameter: OMX_QcomIndexParamPerfLevel");
 if (!dev_get_performance_level(&perflevel)) {
                    DEBUG_PRINT_ERROR("Invalid entry returned from get_performance_level %d",
                        pParam->ePerfLevel);
 } else {
                    pParam->ePerfLevel = (QOMX_VIDEO_PERF_LEVEL)perflevel;
 }
 break;

             }
         case OMX_QcomIndexParamH264VUITimingInfo:
             {
                VALIDATE_OMX_PARAM_DATA(paramData, OMX_QCOM_VIDEO_PARAM_VUI_TIMING_INFO);
                 OMX_U32 enabled;
                 OMX_QCOM_VIDEO_PARAM_VUI_TIMING_INFO *pParam =
                     reinterpret_cast<OMX_QCOM_VIDEO_PARAM_VUI_TIMING_INFO*>(paramData);
                DEBUG_PRINT_LOW("get_parameter: OMX_QcomIndexParamH264VUITimingInfo");
 if (!dev_get_vui_timing_info(&enabled)) {
                    DEBUG_PRINT_ERROR("Invalid entry returned from get_vui_Timing_info %d",
                        pParam->bEnable);
 } else {
                    pParam->bEnable = (OMX_BOOL)enabled;
 }
 break;

             }
         case OMX_QcomIndexParamPeakBitrate:
             {
                VALIDATE_OMX_PARAM_DATA(paramData, OMX_QCOM_VIDEO_PARAM_PEAK_BITRATE);
                 OMX_U32 peakbitrate;
                 OMX_QCOM_VIDEO_PARAM_PEAK_BITRATE *pParam =
                     reinterpret_cast<OMX_QCOM_VIDEO_PARAM_PEAK_BITRATE*>(paramData);
                DEBUG_PRINT_LOW("get_parameter: OMX_QcomIndexParamPeakBitrate");
 if (!dev_get_peak_bitrate(&peakbitrate)) {
                    DEBUG_PRINT_ERROR("Invalid entry returned from get_peak_bitrate %u",
 (unsigned int)pParam->nPeakBitrate);
 } else {
                    pParam->nPeakBitrate = peakbitrate;
 }
 break;

             }
          case QOMX_IndexParamVideoInitialQp:
             {
                VALIDATE_OMX_PARAM_DATA(paramData, QOMX_EXTNINDEX_VIDEO_INITIALQP);
                  QOMX_EXTNINDEX_VIDEO_INITIALQP* initqp =
                      reinterpret_cast<QOMX_EXTNINDEX_VIDEO_INITIALQP *>(paramData);
                      memcpy(initqp, &m_sParamInitqp, sizeof(m_sParamInitqp));
 break;
 }
 case OMX_IndexParamVideoSliceFMO:
 default:
 {
                DEBUG_PRINT_LOW("ERROR: get_parameter: unknown param %08x", paramIndex);
                eRet =OMX_ErrorUnsupportedIndex;
 break;
 }

 }

 return eRet;

}
