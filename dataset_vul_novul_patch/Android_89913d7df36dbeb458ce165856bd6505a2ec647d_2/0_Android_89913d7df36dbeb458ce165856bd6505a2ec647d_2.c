OMX_ERRORTYPE  omx_venc::set_parameter(OMX_IN OMX_HANDLETYPE     hComp,
        OMX_IN OMX_INDEXTYPE paramIndex,
        OMX_IN OMX_PTR        paramData)
{
 (void)hComp;
    OMX_ERRORTYPE eRet = OMX_ErrorNone;


 if (m_state == OMX_StateInvalid) {
        DEBUG_PRINT_ERROR("ERROR: Set Param in Invalid State");
 return OMX_ErrorInvalidState;
 }
 if (paramData == NULL) {
        DEBUG_PRINT_ERROR("ERROR: Get Param in Invalid paramData");
 return OMX_ErrorBadParameter;
 }

 /*set_parameter can be called in loaded state
      or disabled port */
 if (m_state == OMX_StateLoaded
 || m_sInPortDef.bEnabled == OMX_FALSE
 || m_sOutPortDef.bEnabled == OMX_FALSE) {
        DEBUG_PRINT_LOW("Set Parameter called in valid state");
 } else {
        DEBUG_PRINT_ERROR("ERROR: Set Parameter called in Invalid State");
 return OMX_ErrorIncorrectStateOperation;
 }

 switch ((int)paramIndex) {
 case OMX_IndexParamPortDefinition:
 {
                VALIDATE_OMX_PARAM_DATA(paramData, OMX_PARAM_PORTDEFINITIONTYPE);
                OMX_PARAM_PORTDEFINITIONTYPE *portDefn;
                portDefn = (OMX_PARAM_PORTDEFINITIONTYPE *) paramData;
                DEBUG_PRINT_LOW("set_parameter: OMX_IndexParamPortDefinition H= %d, W = %d",
 (int)portDefn->format.video.nFrameHeight,
 (int)portDefn->format.video.nFrameWidth);

 if (PORT_INDEX_IN == portDefn->nPortIndex) {
 if (!dev_is_video_session_supported(portDefn->format.video.nFrameWidth,
                                portDefn->format.video.nFrameHeight)) {
                        DEBUG_PRINT_ERROR("video session not supported");
                        omx_report_unsupported_setting();
 return OMX_ErrorUnsupportedSetting;
 }

                     DEBUG_PRINT_LOW("i/p actual cnt requested = %u", (unsigned int)portDefn->nBufferCountActual);
                     DEBUG_PRINT_LOW("i/p min cnt requested = %u", (unsigned int)portDefn->nBufferCountMin);
                     DEBUG_PRINT_LOW("i/p buffersize requested = %u", (unsigned int)portDefn->nBufferSize);
                    if (portDefn->nBufferCountActual > MAX_NUM_INPUT_BUFFERS) {
                        DEBUG_PRINT_ERROR("ERROR: (In_PORT) actual count (%u) exceeds max(%u)",
                                (unsigned int)portDefn->nBufferCountActual, (unsigned int)MAX_NUM_INPUT_BUFFERS);
                        return OMX_ErrorUnsupportedSetting;
                    }
                     if (portDefn->nBufferCountMin > portDefn->nBufferCountActual) {
                         DEBUG_PRINT_ERROR("ERROR: (In_PORT) Min buffers (%u) > actual count (%u)",
                                 (unsigned int)portDefn->nBufferCountMin, (unsigned int)portDefn->nBufferCountActual);
 return OMX_ErrorUnsupportedSetting;
 }
 if (handle->venc_set_param(paramData,OMX_IndexParamPortDefinition) != true) {
                        DEBUG_PRINT_ERROR("ERROR: venc_set_param input failed");
 return handle->hw_overload ? OMX_ErrorInsufficientResources :
                                OMX_ErrorUnsupportedSetting;
 }

                    DEBUG_PRINT_LOW("i/p previous actual cnt = %u", (unsigned int)m_sInPortDef.nBufferCountActual);
                    DEBUG_PRINT_LOW("i/p previous min cnt = %u", (unsigned int)m_sInPortDef.nBufferCountMin);
                    memcpy(&m_sInPortDef, portDefn,sizeof(OMX_PARAM_PORTDEFINITIONTYPE));

#ifdef _ANDROID_ICS_
 if (portDefn->format.video.eColorFormat ==
 (OMX_COLOR_FORMATTYPE)QOMX_COLOR_FormatAndroidOpaque) {
                        m_sInPortDef.format.video.eColorFormat = (OMX_COLOR_FORMATTYPE)
                            QOMX_COLOR_FORMATYUV420PackedSemiPlanar32m;
 if (!mUseProxyColorFormat) {
 if (!c2d_conv.init()) {
                                DEBUG_PRINT_ERROR("C2D init failed");
 return OMX_ErrorUnsupportedSetting;
 }
                            DEBUG_PRINT_HIGH("C2D init is successful");
 }
                        mUseProxyColorFormat = true;
                        m_input_msg_id = OMX_COMPONENT_GENERATE_ETB_OPQ;
 } else
                        mUseProxyColorFormat = false;
#endif
 /*Query Input Buffer Requirements*/
                    dev_get_buf_req   (&m_sInPortDef.nBufferCountMin,
 &m_sInPortDef.nBufferCountActual,
 &m_sInPortDef.nBufferSize,
                            m_sInPortDef.nPortIndex);

 /*Query ouput Buffer Requirements*/
                    dev_get_buf_req   (&m_sOutPortDef.nBufferCountMin,
 &m_sOutPortDef.nBufferCountActual,
 &m_sOutPortDef.nBufferSize,
                            m_sOutPortDef.nPortIndex);
                    m_sInPortDef.nBufferCountActual = portDefn->nBufferCountActual;
 } else if (PORT_INDEX_OUT == portDefn->nPortIndex) {

                     DEBUG_PRINT_LOW("o/p actual cnt requested = %u", (unsigned int)portDefn->nBufferCountActual);
                     DEBUG_PRINT_LOW("o/p min cnt requested = %u", (unsigned int)portDefn->nBufferCountMin);
                     DEBUG_PRINT_LOW("o/p buffersize requested = %u", (unsigned int)portDefn->nBufferSize);
                    if (portDefn->nBufferCountActual > MAX_NUM_OUTPUT_BUFFERS) {
                        DEBUG_PRINT_ERROR("ERROR: (Out_PORT) actual count (%u) exceeds max(%u)",
                                (unsigned int)portDefn->nBufferCountActual, (unsigned int)MAX_NUM_OUTPUT_BUFFERS);
                        return OMX_ErrorUnsupportedSetting;
                    }
                     if (portDefn->nBufferCountMin > portDefn->nBufferCountActual) {
                         DEBUG_PRINT_ERROR("ERROR: (Out_PORT) Min buffers (%u) > actual count (%u)",
                                 (unsigned int)portDefn->nBufferCountMin, (unsigned int)portDefn->nBufferCountActual);
 return OMX_ErrorUnsupportedSetting;
 }
 if (handle->venc_set_param(paramData,OMX_IndexParamPortDefinition) != true) {
                        DEBUG_PRINT_ERROR("ERROR: venc_set_param output failed");
 return OMX_ErrorUnsupportedSetting;
 }
#ifdef _MSM8974_
 /*Query ouput Buffer Requirements*/
                    dev_get_buf_req(&m_sOutPortDef.nBufferCountMin,
 &m_sOutPortDef.nBufferCountActual,
 &m_sOutPortDef.nBufferSize,
                            m_sOutPortDef.nPortIndex);
#endif
                    memcpy(&m_sOutPortDef,portDefn,sizeof(struct OMX_PARAM_PORTDEFINITIONTYPE));
                    update_profile_level(); //framerate , bitrate

                    DEBUG_PRINT_LOW("o/p previous actual cnt = %u", (unsigned int)m_sOutPortDef.nBufferCountActual);
                    DEBUG_PRINT_LOW("o/p previous min cnt = %u", (unsigned int)m_sOutPortDef.nBufferCountMin);
                    m_sOutPortDef.nBufferCountActual = portDefn->nBufferCountActual;
 } else {
                    DEBUG_PRINT_ERROR("ERROR: Set_parameter: Bad Port idx %d",
 (int)portDefn->nPortIndex);
                    eRet = OMX_ErrorBadPortIndex;
 }
                m_sConfigFramerate.xEncodeFramerate = portDefn->format.video.xFramerate;
                m_sConfigBitrate.nEncodeBitrate = portDefn->format.video.nBitrate;
                m_sParamBitrate.nTargetBitrate = portDefn->format.video.nBitrate;
 }
 break;

 case OMX_IndexParamVideoPortFormat:
 {
                VALIDATE_OMX_PARAM_DATA(paramData, OMX_VIDEO_PARAM_PORTFORMATTYPE);
                OMX_VIDEO_PARAM_PORTFORMATTYPE *portFmt =
 (OMX_VIDEO_PARAM_PORTFORMATTYPE *)paramData;
                DEBUG_PRINT_LOW("set_parameter: OMX_IndexParamVideoPortFormat %d",
                        portFmt->eColorFormat);
 if (PORT_INDEX_IN == portFmt->nPortIndex) {
 if (handle->venc_set_param(paramData,OMX_IndexParamVideoPortFormat) != true) {
 return OMX_ErrorUnsupportedSetting;
 }

                    DEBUG_PRINT_LOW("set_parameter: OMX_IndexParamVideoPortFormat %d",
                            portFmt->eColorFormat);
                    update_profile_level(); //framerate

#ifdef _ANDROID_ICS_
 if (portFmt->eColorFormat ==
 (OMX_COLOR_FORMATTYPE)QOMX_COLOR_FormatAndroidOpaque) {
                        m_sInPortFormat.eColorFormat = (OMX_COLOR_FORMATTYPE)
                            QOMX_COLOR_FORMATYUV420PackedSemiPlanar32m;
 if (!mUseProxyColorFormat) {
 if (!c2d_conv.init()) {
                                DEBUG_PRINT_ERROR("C2D init failed");
 return OMX_ErrorUnsupportedSetting;
 }
                            DEBUG_PRINT_HIGH("C2D init is successful");
 }
                        mUseProxyColorFormat = true;
                        m_input_msg_id = OMX_COMPONENT_GENERATE_ETB_OPQ;
 } else
#endif
 {
                        m_sInPortFormat.eColorFormat = portFmt->eColorFormat;
                        m_sInPortDef.format.video.eColorFormat = portFmt->eColorFormat;
                        m_input_msg_id = OMX_COMPONENT_GENERATE_ETB;
                        mUseProxyColorFormat = false;
 }
                    m_sInPortFormat.xFramerate = portFmt->xFramerate;
 }
 }
 break;
 case OMX_IndexParamVideoInit:
 { //TODO, do we need this index set param
                VALIDATE_OMX_PARAM_DATA(paramData, OMX_PORT_PARAM_TYPE);
                OMX_PORT_PARAM_TYPE* pParam = (OMX_PORT_PARAM_TYPE*)(paramData);
                DEBUG_PRINT_LOW("Set OMX_IndexParamVideoInit called");
 break;
 }

 case OMX_IndexParamVideoBitrate:
 {
                VALIDATE_OMX_PARAM_DATA(paramData, OMX_VIDEO_PARAM_BITRATETYPE);
                OMX_VIDEO_PARAM_BITRATETYPE* pParam = (OMX_VIDEO_PARAM_BITRATETYPE*)paramData;
                DEBUG_PRINT_LOW("set_parameter: OMX_IndexParamVideoBitrate");
 if (handle->venc_set_param(paramData,OMX_IndexParamVideoBitrate) != true) {
 return OMX_ErrorUnsupportedSetting;
 }
                m_sParamBitrate.nTargetBitrate = pParam->nTargetBitrate;
                m_sParamBitrate.eControlRate = pParam->eControlRate;
                update_profile_level(); //bitrate
                m_sConfigBitrate.nEncodeBitrate = pParam->nTargetBitrate;
                m_sInPortDef.format.video.nBitrate = pParam->nTargetBitrate;
                m_sOutPortDef.format.video.nBitrate = pParam->nTargetBitrate;
                DEBUG_PRINT_LOW("bitrate = %u", (unsigned int)m_sOutPortDef.format.video.nBitrate);
 break;
 }
 case OMX_IndexParamVideoMpeg4:
 {
                VALIDATE_OMX_PARAM_DATA(paramData, OMX_VIDEO_PARAM_MPEG4TYPE);
                OMX_VIDEO_PARAM_MPEG4TYPE* pParam = (OMX_VIDEO_PARAM_MPEG4TYPE*)paramData;
                OMX_VIDEO_PARAM_MPEG4TYPE mp4_param;
                memcpy(&mp4_param, pParam, sizeof(struct OMX_VIDEO_PARAM_MPEG4TYPE));
                DEBUG_PRINT_LOW("set_parameter: OMX_IndexParamVideoMpeg4");
 if (pParam->eProfile == OMX_VIDEO_MPEG4ProfileAdvancedSimple) {
#ifdef MAX_RES_1080P
 if (pParam->nBFrames) {
                        DEBUG_PRINT_HIGH("INFO: Only 1 Bframe is supported");
                        mp4_param.nBFrames = 1;
 }
#else
 if (pParam->nBFrames) {
                        DEBUG_PRINT_ERROR("Warning: B frames not supported");
                        mp4_param.nBFrames = 0;
 }
#endif
#ifdef _MSM8974_
 if (pParam->nBFrames || bframes)
                        mp4_param.nBFrames = (pParam->nBFrames > (unsigned int) bframes)? pParam->nBFrames : bframes;
                    DEBUG_PRINT_HIGH("MPEG4: %u BFrames are being set", (unsigned int)mp4_param.nBFrames);
#endif

 } else {
 if (pParam->nBFrames) {
                        DEBUG_PRINT_ERROR("Warning: B frames not supported");
                        mp4_param.nBFrames = 0;
 }
 }
 if (handle->venc_set_param(&mp4_param,OMX_IndexParamVideoMpeg4) != true) {
 return OMX_ErrorUnsupportedSetting;
 }
                memcpy(&m_sParamMPEG4,pParam, sizeof(struct OMX_VIDEO_PARAM_MPEG4TYPE));
                m_sIntraperiod.nPFrames = m_sParamMPEG4.nPFrames;
 if (pParam->nBFrames || bframes)
                    m_sIntraperiod.nBFrames = m_sParamMPEG4.nBFrames = mp4_param.nBFrames;
 else
                m_sIntraperiod.nBFrames = m_sParamMPEG4.nBFrames;
 break;
 }
 case OMX_IndexParamVideoH263:
 {
                OMX_VIDEO_PARAM_H263TYPE* pParam = (OMX_VIDEO_PARAM_H263TYPE*)paramData;
                DEBUG_PRINT_LOW("set_parameter: OMX_IndexParamVideoH263");
 if (handle->venc_set_param(paramData,OMX_IndexParamVideoH263) != true) {
 return OMX_ErrorUnsupportedSetting;
 }
                memcpy(&m_sParamH263,pParam, sizeof(struct OMX_VIDEO_PARAM_H263TYPE));
                m_sIntraperiod.nPFrames = m_sParamH263.nPFrames;
                m_sIntraperiod.nBFrames = m_sParamH263.nBFrames;
 break;
 }
 case OMX_IndexParamVideoAvc:
 {
                VALIDATE_OMX_PARAM_DATA(paramData, OMX_VIDEO_PARAM_AVCTYPE);
                OMX_VIDEO_PARAM_AVCTYPE* pParam = (OMX_VIDEO_PARAM_AVCTYPE*)paramData;
                OMX_VIDEO_PARAM_AVCTYPE avc_param;
                memcpy(&avc_param, pParam, sizeof( struct OMX_VIDEO_PARAM_AVCTYPE));
                DEBUG_PRINT_LOW("set_parameter: OMX_IndexParamVideoAvc");

 if ((pParam->eProfile == OMX_VIDEO_AVCProfileHigh)||
 (pParam->eProfile == OMX_VIDEO_AVCProfileMain)) {
#ifdef MAX_RES_1080P
 if (pParam->nBFrames) {
                        DEBUG_PRINT_HIGH("INFO: Only 1 Bframe is supported");
                        avc_param.nBFrames = 1;
 }
 if (pParam->nRefFrames != 2) {
                        DEBUG_PRINT_ERROR("Warning: 2 RefFrames are needed, changing RefFrames from %u to 2", (unsigned int)pParam->nRefFrames);
                        avc_param.nRefFrames = 2;
 }
#else
 if (pParam->nBFrames) {
                        DEBUG_PRINT_ERROR("Warning: B frames not supported");
                        avc_param.nBFrames = 0;
 }
 if (pParam->nRefFrames != 1) {
                        DEBUG_PRINT_ERROR("Warning: Only 1 RefFrame is supported, changing RefFrame from %u to 1)", (unsigned int)pParam->nRefFrames);
                        avc_param.nRefFrames = 1;
 }
#endif
#ifdef _MSM8974_
 if (pParam->nBFrames || bframes) {
                        avc_param.nBFrames = (pParam->nBFrames > (unsigned int) bframes)? pParam->nBFrames : bframes;
                        avc_param.nRefFrames = (avc_param.nBFrames < 4)? avc_param.nBFrames + 1 : 4;
 }
                    DEBUG_PRINT_HIGH("AVC: RefFrames: %u, BFrames: %u", (unsigned int)avc_param.nRefFrames, (unsigned int)avc_param.nBFrames);

                    avc_param.bEntropyCodingCABAC = (OMX_BOOL)(avc_param.bEntropyCodingCABAC && entropy);
                    avc_param.nCabacInitIdc = entropy ? avc_param.nCabacInitIdc : 0;
#endif
 } else {
 if (pParam->nRefFrames != 1) {
                        DEBUG_PRINT_ERROR("Warning: Only 1 RefFrame is supported, changing RefFrame from %u to 1)", (unsigned int)pParam->nRefFrames);
                        avc_param.nRefFrames = 1;
 }
 if (pParam->nBFrames) {
                        DEBUG_PRINT_ERROR("Warning: B frames not supported");
                        avc_param.nBFrames = 0;
 }
 }
 if (handle->venc_set_param(&avc_param,OMX_IndexParamVideoAvc) != true) {
 return OMX_ErrorUnsupportedSetting;
 }
                memcpy(&m_sParamAVC,pParam, sizeof(struct OMX_VIDEO_PARAM_AVCTYPE));
                m_sIntraperiod.nPFrames = m_sParamAVC.nPFrames;
 if (pParam->nBFrames || bframes)
                    m_sIntraperiod.nBFrames = m_sParamAVC.nBFrames = avc_param.nBFrames;
 else
                m_sIntraperiod.nBFrames = m_sParamAVC.nBFrames;
 break;
 }
 case (OMX_INDEXTYPE)OMX_IndexParamVideoVp8:
 {
                VALIDATE_OMX_PARAM_DATA(paramData, OMX_VIDEO_PARAM_VP8TYPE);
                OMX_VIDEO_PARAM_VP8TYPE* pParam = (OMX_VIDEO_PARAM_VP8TYPE*)paramData;
                OMX_VIDEO_PARAM_VP8TYPE vp8_param;
                DEBUG_PRINT_LOW("set_parameter: OMX_IndexParamVideoVp8");
 if (pParam->nDCTPartitions != m_sParamVP8.nDCTPartitions ||
                    pParam->bErrorResilientMode != m_sParamVP8.bErrorResilientMode) {
                    DEBUG_PRINT_ERROR("VP8 doesn't support nDCTPartitions or bErrorResilientMode");
 }
                memcpy(&vp8_param, pParam, sizeof( struct OMX_VIDEO_PARAM_VP8TYPE));
 if (handle->venc_set_param(&vp8_param, (OMX_INDEXTYPE)OMX_IndexParamVideoVp8) != true) {
 return OMX_ErrorUnsupportedSetting;
 }
                memcpy(&m_sParamVP8,pParam, sizeof(struct OMX_VIDEO_PARAM_VP8TYPE));
 break;
 }
 case (OMX_INDEXTYPE)OMX_IndexParamVideoHevc:
 {
                VALIDATE_OMX_PARAM_DATA(paramData, OMX_VIDEO_PARAM_HEVCTYPE);
                OMX_VIDEO_PARAM_HEVCTYPE* pParam = (OMX_VIDEO_PARAM_HEVCTYPE*)paramData;
                OMX_VIDEO_PARAM_HEVCTYPE hevc_param;
                DEBUG_PRINT_LOW("set_parameter: OMX_IndexParamVideoHevc");
                memcpy(&hevc_param, pParam, sizeof( struct OMX_VIDEO_PARAM_HEVCTYPE));
 if (handle->venc_set_param(&hevc_param, (OMX_INDEXTYPE)OMX_IndexParamVideoHevc) != true) {
                    DEBUG_PRINT_ERROR("Failed : set_parameter: OMX_IndexParamVideoHevc");
 return OMX_ErrorUnsupportedSetting;
 }
                memcpy(&m_sParamHEVC, pParam, sizeof(struct OMX_VIDEO_PARAM_HEVCTYPE));
 break;
 }
 case OMX_IndexParamVideoProfileLevelCurrent:
 {
                VALIDATE_OMX_PARAM_DATA(paramData, OMX_VIDEO_PARAM_PROFILELEVELTYPE);
                OMX_VIDEO_PARAM_PROFILELEVELTYPE* pParam = (OMX_VIDEO_PARAM_PROFILELEVELTYPE*)paramData;
                DEBUG_PRINT_LOW("set_parameter: OMX_IndexParamVideoProfileLevelCurrent");
 if (handle->venc_set_param(pParam,OMX_IndexParamVideoProfileLevelCurrent) != true) {
                    DEBUG_PRINT_ERROR("set_parameter: OMX_IndexParamVideoProfileLevelCurrent failed for Profile: %u "
 "Level :%u", (unsigned int)pParam->eProfile, (unsigned int)pParam->eLevel);
 return OMX_ErrorUnsupportedSetting;
 }
                m_sParamProfileLevel.eProfile = pParam->eProfile;
                m_sParamProfileLevel.eLevel = pParam->eLevel;

 if (!strncmp((char *)m_nkind, "OMX.qcom.video.encoder.mpeg4",\
                            OMX_MAX_STRINGNAME_SIZE)) {
                    m_sParamMPEG4.eProfile = (OMX_VIDEO_MPEG4PROFILETYPE)m_sParamProfileLevel.eProfile;
                    m_sParamMPEG4.eLevel = (OMX_VIDEO_MPEG4LEVELTYPE)m_sParamProfileLevel.eLevel;
                    DEBUG_PRINT_LOW("MPEG4 profile = %d, level = %d", m_sParamMPEG4.eProfile,
                            m_sParamMPEG4.eLevel);
 } else if (!strncmp((char *)m_nkind, "OMX.qcom.video.encoder.h263",\
                            OMX_MAX_STRINGNAME_SIZE)) {
                    m_sParamH263.eProfile = (OMX_VIDEO_H263PROFILETYPE)m_sParamProfileLevel.eProfile;
                    m_sParamH263.eLevel = (OMX_VIDEO_H263LEVELTYPE)m_sParamProfileLevel.eLevel;
                    DEBUG_PRINT_LOW("H263 profile = %d, level = %d", m_sParamH263.eProfile,
                            m_sParamH263.eLevel);
 } else if (!strncmp((char *)m_nkind, "OMX.qcom.video.encoder.avc",\
                            OMX_MAX_STRINGNAME_SIZE)) {
                    m_sParamAVC.eProfile = (OMX_VIDEO_AVCPROFILETYPE)m_sParamProfileLevel.eProfile;
                    m_sParamAVC.eLevel = (OMX_VIDEO_AVCLEVELTYPE)m_sParamProfileLevel.eLevel;
                    DEBUG_PRINT_LOW("AVC profile = %d, level = %d", m_sParamAVC.eProfile,
                            m_sParamAVC.eLevel);
 } else if (!strncmp((char *)m_nkind, "OMX.qcom.video.encoder.avc.secure",\
                            OMX_MAX_STRINGNAME_SIZE)) {
                    m_sParamAVC.eProfile = (OMX_VIDEO_AVCPROFILETYPE)m_sParamProfileLevel.eProfile;
                    m_sParamAVC.eLevel = (OMX_VIDEO_AVCLEVELTYPE)m_sParamProfileLevel.eLevel;
                    DEBUG_PRINT_LOW("\n AVC profile = %d, level = %d", m_sParamAVC.eProfile,
                            m_sParamAVC.eLevel);
 }
 else if (!strncmp((char*)m_nkind, "OMX.qcom.video.encoder.vp8",\
                            OMX_MAX_STRINGNAME_SIZE)) {
                    m_sParamVP8.eProfile = (OMX_VIDEO_VP8PROFILETYPE)m_sParamProfileLevel.eProfile;
                    m_sParamVP8.eLevel = (OMX_VIDEO_VP8LEVELTYPE)m_sParamProfileLevel.eLevel;
                    DEBUG_PRINT_LOW("VP8 profile = %d, level = %d", m_sParamVP8.eProfile,
                            m_sParamVP8.eLevel);
 }
 else if (!strncmp((char*)m_nkind, "OMX.qcom.video.encoder.hevc",\
                            OMX_MAX_STRINGNAME_SIZE)) {
                    m_sParamHEVC.eProfile = (OMX_VIDEO_HEVCPROFILETYPE)m_sParamProfileLevel.eProfile;
                    m_sParamHEVC.eLevel = (OMX_VIDEO_HEVCLEVELTYPE)m_sParamProfileLevel.eLevel;
                    DEBUG_PRINT_LOW("HEVC profile = %d, level = %d", m_sParamHEVC.eProfile,
                            m_sParamHEVC.eLevel);
 }

 break;
 }
 case OMX_IndexParamStandardComponentRole:
 {
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

 if (!strncmp((char*)m_nkind, "OMX.qcom.video.encoder.avc",OMX_MAX_STRINGNAME_SIZE)) {
 if (!strncmp((char*)comp_role->cRole,"video_encoder.avc",OMX_MAX_STRINGNAME_SIZE)) {
                        strlcpy((char*)m_cRole,"video_encoder.avc",OMX_MAX_STRINGNAME_SIZE);
 } else {
                        DEBUG_PRINT_ERROR("ERROR: Setparameter: unknown Index %s", comp_role->cRole);
                        eRet =OMX_ErrorUnsupportedSetting;
 }
 } else if (!strncmp((char*)m_nkind, "OMX.qcom.video.encoder.avc.secure",OMX_MAX_STRINGNAME_SIZE)) {
 if (!strncmp((char*)comp_role->cRole,"video_encoder.avc",OMX_MAX_STRINGNAME_SIZE)) {
                        strlcpy((char*)m_cRole,"video_encoder.avc",OMX_MAX_STRINGNAME_SIZE);
 } else {
                        DEBUG_PRINT_ERROR("ERROR: Setparameter: unknown Index %s\n", comp_role->cRole);
                        eRet =OMX_ErrorUnsupportedSetting;
 }
 } else if (!strncmp((char*)m_nkind, "OMX.qcom.video.encoder.mpeg4",OMX_MAX_STRINGNAME_SIZE)) {
 if (!strncmp((const char*)comp_role->cRole,"video_encoder.mpeg4",OMX_MAX_STRINGNAME_SIZE)) {
                        strlcpy((char*)m_cRole,"video_encoder.mpeg4",OMX_MAX_STRINGNAME_SIZE);
 } else {
                        DEBUG_PRINT_ERROR("ERROR: Setparameter: unknown Index %s", comp_role->cRole);
                        eRet = OMX_ErrorUnsupportedSetting;
 }
 } else if (!strncmp((char*)m_nkind, "OMX.qcom.video.encoder.h263",OMX_MAX_STRINGNAME_SIZE)) {
 if (!strncmp((const char*)comp_role->cRole,"video_encoder.h263",OMX_MAX_STRINGNAME_SIZE)) {
                        strlcpy((char*)m_cRole,"video_encoder.h263",OMX_MAX_STRINGNAME_SIZE);
 } else {
                        DEBUG_PRINT_ERROR("ERROR: Setparameter: unknown Index %s", comp_role->cRole);
                        eRet =OMX_ErrorUnsupportedSetting;
 }
 }
#ifdef _MSM8974_
 else if (!strncmp((char*)m_nkind, "OMX.qcom.video.encoder.vp8",OMX_MAX_STRINGNAME_SIZE)) {
 if (!strncmp((const char*)comp_role->cRole,"video_encoder.vp8",OMX_MAX_STRINGNAME_SIZE)) {
                        strlcpy((char*)m_cRole,"video_encoder.vp8",OMX_MAX_STRINGNAME_SIZE);
 } else {
                        DEBUG_PRINT_ERROR("ERROR: Setparameter: unknown Index %s", comp_role->cRole);
                        eRet =OMX_ErrorUnsupportedSetting;
 }
 }
#endif
 else if (!strncmp((char*)m_nkind, "OMX.qcom.video.encoder.hevc",OMX_MAX_STRINGNAME_SIZE)) {
 if (!strncmp((const char*)comp_role->cRole,"video_encoder.hevc",OMX_MAX_STRINGNAME_SIZE)) {
                        strlcpy((char*)m_cRole,"video_encoder.hevc",OMX_MAX_STRINGNAME_SIZE);
 } else {
                        DEBUG_PRINT_ERROR("ERROR: Setparameter: unknown Index %s", comp_role->cRole);
                        eRet = OMX_ErrorUnsupportedSetting;
 }
 }

 else {
                    DEBUG_PRINT_ERROR("ERROR: Setparameter: unknown param %s", m_nkind);
                    eRet = OMX_ErrorInvalidComponentName;
 }
 break;
 }

 case OMX_IndexParamPriorityMgmt:
 {
                VALIDATE_OMX_PARAM_DATA(paramData, OMX_PRIORITYMGMTTYPE);
                DEBUG_PRINT_LOW("set_parameter: OMX_IndexParamPriorityMgmt");
 if (m_state != OMX_StateLoaded) {
                    DEBUG_PRINT_ERROR("ERROR: Set Parameter called in Invalid State");
 return OMX_ErrorIncorrectStateOperation;
 }
                OMX_PRIORITYMGMTTYPE *priorityMgmtype = (OMX_PRIORITYMGMTTYPE*) paramData;
                DEBUG_PRINT_LOW("set_parameter: OMX_IndexParamPriorityMgmt %u",
 (unsigned int)priorityMgmtype->nGroupID);

                DEBUG_PRINT_LOW("set_parameter: priorityMgmtype %u",
 (unsigned int)priorityMgmtype->nGroupPriority);

                m_sPriorityMgmt.nGroupID = priorityMgmtype->nGroupID;
                m_sPriorityMgmt.nGroupPriority = priorityMgmtype->nGroupPriority;

 break;
 }

 case OMX_IndexParamCompBufferSupplier:
 {
                VALIDATE_OMX_PARAM_DATA(paramData, OMX_PARAM_BUFFERSUPPLIERTYPE);
                DEBUG_PRINT_LOW("set_parameter: OMX_IndexParamCompBufferSupplier");
                OMX_PARAM_BUFFERSUPPLIERTYPE *bufferSupplierType = (OMX_PARAM_BUFFERSUPPLIERTYPE*) paramData;
                DEBUG_PRINT_LOW("set_parameter: OMX_IndexParamCompBufferSupplier %d",
                        bufferSupplierType->eBufferSupplier);
 if (bufferSupplierType->nPortIndex == 0 || bufferSupplierType->nPortIndex ==1)
                    m_sInBufSupplier.eBufferSupplier = bufferSupplierType->eBufferSupplier;

 else

                    eRet = OMX_ErrorBadPortIndex;

 break;

 }
 case OMX_IndexParamVideoQuantization:
 {
                VALIDATE_OMX_PARAM_DATA(paramData, OMX_VIDEO_PARAM_QUANTIZATIONTYPE);
                DEBUG_PRINT_LOW("set_parameter: OMX_IndexParamVideoQuantization");
                OMX_VIDEO_PARAM_QUANTIZATIONTYPE *session_qp = (OMX_VIDEO_PARAM_QUANTIZATIONTYPE*) paramData;
 if (session_qp->nPortIndex == PORT_INDEX_OUT) {
 if (handle->venc_set_param(paramData, OMX_IndexParamVideoQuantization) != true) {
 return OMX_ErrorUnsupportedSetting;
 }
                    m_sSessionQuantization.nQpI = session_qp->nQpI;
                    m_sSessionQuantization.nQpP = session_qp->nQpP;
                    m_sSessionQuantization.nQpB = session_qp->nQpB;
 } else {
                    DEBUG_PRINT_ERROR("ERROR: Unsupported port Index for Session QP setting");
                    eRet = OMX_ErrorBadPortIndex;
 }
 break;
 }

 case OMX_QcomIndexParamVideoQPRange:
 {
                VALIDATE_OMX_PARAM_DATA(paramData, OMX_QCOM_VIDEO_PARAM_QPRANGETYPE);
                DEBUG_PRINT_LOW("set_parameter: OMX_QcomIndexParamVideoQPRange");
                OMX_QCOM_VIDEO_PARAM_QPRANGETYPE *qp_range = (OMX_QCOM_VIDEO_PARAM_QPRANGETYPE*) paramData;
 if (qp_range->nPortIndex == PORT_INDEX_OUT) {
 if (handle->venc_set_param(paramData,
 (OMX_INDEXTYPE)OMX_QcomIndexParamVideoQPRange) != true) {
 return OMX_ErrorUnsupportedSetting;
 }
                    m_sSessionQPRange.minQP= qp_range->minQP;
                    m_sSessionQPRange.maxQP= qp_range->maxQP;
 } else {
                    DEBUG_PRINT_ERROR("ERROR: Unsupported port Index for QP range setting");
                    eRet = OMX_ErrorBadPortIndex;
 }
 break;
 }

 case OMX_QcomIndexPortDefn:
 {
                VALIDATE_OMX_PARAM_DATA(paramData, OMX_QCOM_PARAM_PORTDEFINITIONTYPE);
                OMX_QCOM_PARAM_PORTDEFINITIONTYPE* pParam =
 (OMX_QCOM_PARAM_PORTDEFINITIONTYPE*)paramData;
                DEBUG_PRINT_LOW("set_parameter: OMX_QcomIndexPortDefn");
 if (pParam->nPortIndex == (OMX_U32)PORT_INDEX_IN) {
 if (pParam->nMemRegion > OMX_QCOM_MemRegionInvalid &&
                            pParam->nMemRegion < OMX_QCOM_MemRegionMax) {
                        m_use_input_pmem = OMX_TRUE;
 } else {
                        m_use_input_pmem = OMX_FALSE;
 }
 } else if (pParam->nPortIndex == (OMX_U32)PORT_INDEX_OUT) {
 if (pParam->nMemRegion > OMX_QCOM_MemRegionInvalid &&
                            pParam->nMemRegion < OMX_QCOM_MemRegionMax) {
                        m_use_output_pmem = OMX_TRUE;
 } else {
                        m_use_output_pmem = OMX_FALSE;
 }
 } else {
                    DEBUG_PRINT_ERROR("ERROR: SetParameter called on unsupported Port Index for QcomPortDefn");
 return OMX_ErrorBadPortIndex;
 }
 break;
 }

 case OMX_IndexParamVideoErrorCorrection:
 {
                VALIDATE_OMX_PARAM_DATA(paramData, OMX_VIDEO_PARAM_ERRORCORRECTIONTYPE);
                DEBUG_PRINT_LOW("OMX_IndexParamVideoErrorCorrection");
                OMX_VIDEO_PARAM_ERRORCORRECTIONTYPE* pParam =
 (OMX_VIDEO_PARAM_ERRORCORRECTIONTYPE*)paramData;
 if (!handle->venc_set_param(paramData, OMX_IndexParamVideoErrorCorrection)) {
                    DEBUG_PRINT_ERROR("ERROR: Request for setting Error Resilience failed");
 return OMX_ErrorUnsupportedSetting;
 }
                memcpy(&m_sErrorCorrection,pParam, sizeof(m_sErrorCorrection));
 break;
 }
 case OMX_IndexParamVideoIntraRefresh:
 {
                VALIDATE_OMX_PARAM_DATA(paramData, OMX_VIDEO_PARAM_INTRAREFRESHTYPE);
                DEBUG_PRINT_LOW("set_param:OMX_IndexParamVideoIntraRefresh");
                OMX_VIDEO_PARAM_INTRAREFRESHTYPE* pParam =
 (OMX_VIDEO_PARAM_INTRAREFRESHTYPE*)paramData;
 if (!handle->venc_set_param(paramData,OMX_IndexParamVideoIntraRefresh)) {
                    DEBUG_PRINT_ERROR("ERROR: Request for setting intra refresh failed");
 return OMX_ErrorUnsupportedSetting;
 }
                memcpy(&m_sIntraRefresh, pParam, sizeof(m_sIntraRefresh));
 break;
 }
#ifdef _ANDROID_ICS_
 case OMX_QcomIndexParamVideoMetaBufferMode:
 {
                VALIDATE_OMX_PARAM_DATA(paramData, StoreMetaDataInBuffersParams);
 StoreMetaDataInBuffersParams *pParam =
 (StoreMetaDataInBuffersParams*)paramData;
                DEBUG_PRINT_HIGH("set_parameter:OMX_QcomIndexParamVideoMetaBufferMode: "
 "port_index = %u, meta_mode = %d", (unsigned int)pParam->nPortIndex, pParam->bStoreMetaData);
 if (pParam->nPortIndex == PORT_INDEX_IN) {
 if (pParam->bStoreMetaData != meta_mode_enable) {
 if (!handle->venc_set_meta_mode(pParam->bStoreMetaData)) {
                            DEBUG_PRINT_ERROR("ERROR: set Metabuffer mode %d fail",
                                    pParam->bStoreMetaData);
 return OMX_ErrorUnsupportedSetting;
 }
                        meta_mode_enable = pParam->bStoreMetaData;
 if (meta_mode_enable) {
                            m_sInPortDef.nBufferCountActual = m_sInPortDef.nBufferCountMin;
 if (handle->venc_set_param(&m_sInPortDef,OMX_IndexParamPortDefinition) != true) {
                                DEBUG_PRINT_ERROR("ERROR: venc_set_param input failed");
 return OMX_ErrorUnsupportedSetting;
 }
 } else {
 /*TODO: reset encoder driver Meta mode*/
                            dev_get_buf_req   (&m_sOutPortDef.nBufferCountMin,
 &m_sOutPortDef.nBufferCountActual,
 &m_sOutPortDef.nBufferSize,
                                    m_sOutPortDef.nPortIndex);
 }
 }
 } else if (pParam->nPortIndex == PORT_INDEX_OUT && secure_session) {
 if (pParam->bStoreMetaData != meta_mode_enable) {
 if (!handle->venc_set_meta_mode(pParam->bStoreMetaData)) {
                            DEBUG_PRINT_ERROR("\nERROR: set Metabuffer mode %d fail",
                                    pParam->bStoreMetaData);
 return OMX_ErrorUnsupportedSetting;
 }
                        meta_mode_enable = pParam->bStoreMetaData;
 }
 } else {
                    DEBUG_PRINT_ERROR("set_parameter: metamode is "
 "valid for input port only");
                    eRet = OMX_ErrorUnsupportedIndex;
 }
 }
 break;
#endif
#if !defined(MAX_RES_720P) || defined(_MSM8974_)
 case OMX_QcomIndexParamIndexExtraDataType:
 {
                VALIDATE_OMX_PARAM_DATA(paramData, QOMX_INDEXEXTRADATATYPE);
                DEBUG_PRINT_HIGH("set_parameter: OMX_QcomIndexParamIndexExtraDataType");
                QOMX_INDEXEXTRADATATYPE *pParam = (QOMX_INDEXEXTRADATATYPE *)paramData;
 bool enable = false;
                OMX_U32 mask = 0;

 if (pParam->nIndex == (OMX_INDEXTYPE)OMX_ExtraDataVideoEncoderSliceInfo) {
 if (pParam->nPortIndex == PORT_INDEX_OUT) {
                        mask = VEN_EXTRADATA_SLICEINFO;

                        DEBUG_PRINT_HIGH("SliceInfo extradata %s",
 ((pParam->bEnabled == OMX_TRUE) ? "enabled" : "disabled"));
 } else {
                        DEBUG_PRINT_ERROR("set_parameter: Slice information is "
 "valid for output port only");
                        eRet = OMX_ErrorUnsupportedIndex;
 break;
 }
 } else if (pParam->nIndex == (OMX_INDEXTYPE)OMX_ExtraDataVideoEncoderMBInfo) {
 if (pParam->nPortIndex == PORT_INDEX_OUT) {
                        mask = VEN_EXTRADATA_MBINFO;

                        DEBUG_PRINT_HIGH("MBInfo extradata %s",
 ((pParam->bEnabled == OMX_TRUE) ? "enabled" : "disabled"));
 } else {
                        DEBUG_PRINT_ERROR("set_parameter: MB information is "
 "valid for output port only");
                        eRet = OMX_ErrorUnsupportedIndex;
 break;
 }
 }
#ifndef _MSM8974_
 else if (pParam->nIndex == (OMX_INDEXTYPE)OMX_ExtraDataVideoLTRInfo) {
 if (pParam->nPortIndex == PORT_INDEX_OUT) {
 if (pParam->bEnabled == OMX_TRUE)
                            mask = VEN_EXTRADATA_LTRINFO;

                        DEBUG_PRINT_HIGH("LTRInfo extradata %s",
 ((pParam->bEnabled == OMX_TRUE) ? "enabled" : "disabled"));
 } else {
                        DEBUG_PRINT_ERROR("set_parameter: LTR information is "
 "valid for output port only");
                        eRet = OMX_ErrorUnsupportedIndex;
 break;
 }
 }
#endif
 else {
                    DEBUG_PRINT_ERROR("set_parameter: unsupported extrdata index (%x)",
                            pParam->nIndex);
                    eRet = OMX_ErrorUnsupportedIndex;
 break;
 }


 if (pParam->bEnabled == OMX_TRUE)
                    m_sExtraData |= mask;
 else
                    m_sExtraData &= ~mask;

                enable = !!(m_sExtraData & mask);
 if (handle->venc_set_param(&enable,
 (OMX_INDEXTYPE)pParam->nIndex) != true) {
                    DEBUG_PRINT_ERROR("ERROR: Setting Extradata (%x) failed", pParam->nIndex);
 return OMX_ErrorUnsupportedSetting;
 } else {
                    m_sOutPortDef.nPortIndex = PORT_INDEX_OUT;
                    dev_get_buf_req(&m_sOutPortDef.nBufferCountMin,
 &m_sOutPortDef.nBufferCountActual,
 &m_sOutPortDef.nBufferSize,
                            m_sOutPortDef.nPortIndex);
                    DEBUG_PRINT_HIGH("updated out_buf_req: buffer cnt=%u, "
 "count min=%u, buffer size=%u",
 (unsigned int)m_sOutPortDef.nBufferCountActual,
 (unsigned int)m_sOutPortDef.nBufferCountMin,
 (unsigned int)m_sOutPortDef.nBufferSize);
 }
 break;
 }
 case QOMX_IndexParamVideoLTRMode:
 {
                VALIDATE_OMX_PARAM_DATA(paramData, QOMX_VIDEO_PARAM_LTRMODE_TYPE);
                QOMX_VIDEO_PARAM_LTRMODE_TYPE* pParam =
 (QOMX_VIDEO_PARAM_LTRMODE_TYPE*)paramData;
 if (!handle->venc_set_param(paramData, (OMX_INDEXTYPE)QOMX_IndexParamVideoLTRMode)) {
                    DEBUG_PRINT_ERROR("ERROR: Setting LTR mode failed");
 return OMX_ErrorUnsupportedSetting;
 }
                memcpy(&m_sParamLTRMode, pParam, sizeof(m_sParamLTRMode));
 break;
 }
 case QOMX_IndexParamVideoLTRCount:
 {
                VALIDATE_OMX_PARAM_DATA(paramData, QOMX_VIDEO_PARAM_LTRCOUNT_TYPE);
                QOMX_VIDEO_PARAM_LTRCOUNT_TYPE* pParam =
 (QOMX_VIDEO_PARAM_LTRCOUNT_TYPE*)paramData;
 if (!handle->venc_set_param(paramData, (OMX_INDEXTYPE)QOMX_IndexParamVideoLTRCount)) {
                    DEBUG_PRINT_ERROR("ERROR: Setting LTR count failed");
 return OMX_ErrorUnsupportedSetting;
 }
                memcpy(&m_sParamLTRCount, pParam, sizeof(m_sParamLTRCount));
 break;
 }
#endif
 case OMX_QcomIndexParamVideoMaxAllowedBitrateCheck:
 {
                VALIDATE_OMX_PARAM_DATA(paramData, QOMX_EXTNINDEX_PARAMTYPE);
                QOMX_EXTNINDEX_PARAMTYPE* pParam =
 (QOMX_EXTNINDEX_PARAMTYPE*)paramData;
 if (pParam->nPortIndex == PORT_INDEX_OUT) {
                    handle->m_max_allowed_bitrate_check =
 ((pParam->bEnable == OMX_TRUE) ? true : false);
                    DEBUG_PRINT_HIGH("set_parameter: max allowed bitrate check %s",
 ((pParam->bEnable == OMX_TRUE) ? "enabled" : "disabled"));
 } else {
                    DEBUG_PRINT_ERROR("ERROR: OMX_QcomIndexParamVideoMaxAllowedBitrateCheck "
 " called on wrong port(%u)", (unsigned int)pParam->nPortIndex);
 return OMX_ErrorBadPortIndex;
 }
 break;
 }
#ifdef MAX_RES_1080P
 case OMX_QcomIndexEnableSliceDeliveryMode:
 {
                VALIDATE_OMX_PARAM_DATA(paramData, QOMX_EXTNINDEX_PARAMTYPE);
                QOMX_EXTNINDEX_PARAMTYPE* pParam =
 (QOMX_EXTNINDEX_PARAMTYPE*)paramData;
 if (pParam->nPortIndex == PORT_INDEX_OUT) {
 if (!handle->venc_set_param(paramData,
 (OMX_INDEXTYPE)OMX_QcomIndexEnableSliceDeliveryMode)) {
                        DEBUG_PRINT_ERROR("ERROR: Request for setting slice delivery mode failed");
 return OMX_ErrorUnsupportedSetting;
 }
 } else {
                    DEBUG_PRINT_ERROR("ERROR: OMX_QcomIndexEnableSliceDeliveryMode "
 "called on wrong port(%u)", (unsigned int)pParam->nPortIndex);
 return OMX_ErrorBadPortIndex;
 }
 break;
 }
#endif
 case OMX_QcomIndexEnableH263PlusPType:
 {
                VALIDATE_OMX_PARAM_DATA(paramData, QOMX_EXTNINDEX_PARAMTYPE);
                QOMX_EXTNINDEX_PARAMTYPE* pParam =
 (QOMX_EXTNINDEX_PARAMTYPE*)paramData;
                DEBUG_PRINT_LOW("OMX_QcomIndexEnableH263PlusPType");
 if (pParam->nPortIndex == PORT_INDEX_OUT) {
 if (!handle->venc_set_param(paramData,
 (OMX_INDEXTYPE)OMX_QcomIndexEnableH263PlusPType)) {
                        DEBUG_PRINT_ERROR("ERROR: Request for setting PlusPType failed");
 return OMX_ErrorUnsupportedSetting;
 }
 } else {
                    DEBUG_PRINT_ERROR("ERROR: OMX_QcomIndexEnableH263PlusPType "
 "called on wrong port(%u)", (unsigned int)pParam->nPortIndex);
 return OMX_ErrorBadPortIndex;
 }
 break;
 }
 case OMX_QcomIndexParamSequenceHeaderWithIDR:
 {
                VALIDATE_OMX_PARAM_DATA(paramData, PrependSPSPPSToIDRFramesParams);
 if(!handle->venc_set_param(paramData,
 (OMX_INDEXTYPE)OMX_QcomIndexParamSequenceHeaderWithIDR)) {
                    DEBUG_PRINT_ERROR("%s: %s",
 "OMX_QComIndexParamSequenceHeaderWithIDR:",
 "request for inband sps/pps failed.");
 return OMX_ErrorUnsupportedSetting;
 }
 break;
 }
 case OMX_QcomIndexParamH264AUDelimiter:
 {
                VALIDATE_OMX_PARAM_DATA(paramData, OMX_QCOM_VIDEO_CONFIG_H264_AUD);
 if(!handle->venc_set_param(paramData,
 (OMX_INDEXTYPE)OMX_QcomIndexParamH264AUDelimiter)) {
                    DEBUG_PRINT_ERROR("%s: %s",
 "OMX_QComIndexParamh264AUDelimiter:",
 "request for AU Delimiters failed.");
 return OMX_ErrorUnsupportedSetting;
 }
 break;
 }
 case OMX_QcomIndexHierarchicalStructure:
 {
                VALIDATE_OMX_PARAM_DATA(paramData, QOMX_VIDEO_HIERARCHICALLAYERS);
                QOMX_VIDEO_HIERARCHICALLAYERS* pParam =
 (QOMX_VIDEO_HIERARCHICALLAYERS*)paramData;
                DEBUG_PRINT_LOW("OMX_QcomIndexHierarchicalStructure");
 if (pParam->nPortIndex == PORT_INDEX_OUT) {
 if (!handle->venc_set_param(paramData,
 (OMX_INDEXTYPE)OMX_QcomIndexHierarchicalStructure)) {
                        DEBUG_PRINT_ERROR("ERROR: Request for setting PlusPType failed");
 return OMX_ErrorUnsupportedSetting;
 }
 if((pParam->eHierarchicalCodingType == QOMX_HIERARCHICALCODING_B) && pParam->nNumLayers)
                    hier_b_enabled = true;
                    m_sHierLayers.nNumLayers = pParam->nNumLayers;
                    m_sHierLayers.eHierarchicalCodingType = pParam->eHierarchicalCodingType;
 } else {
                    DEBUG_PRINT_ERROR("ERROR: OMX_QcomIndexHierarchicalStructure called on wrong port(%u)",
 (unsigned int)pParam->nPortIndex);
 return OMX_ErrorBadPortIndex;
 }
 break;

 }
 case OMX_QcomIndexParamPerfLevel:
 {
                VALIDATE_OMX_PARAM_DATA(paramData, OMX_QCOM_VIDEO_PARAM_PERF_LEVEL);
 if (!handle->venc_set_param(paramData,
 (OMX_INDEXTYPE) OMX_QcomIndexParamPerfLevel)) {
                    DEBUG_PRINT_ERROR("ERROR: Setting performance level");
 return OMX_ErrorUnsupportedSetting;
 }
 break;
 }
 case OMX_QcomIndexParamH264VUITimingInfo:
 {
                VALIDATE_OMX_PARAM_DATA(paramData, OMX_QCOM_VIDEO_PARAM_VUI_TIMING_INFO);
 if (!handle->venc_set_param(paramData,
 (OMX_INDEXTYPE) OMX_QcomIndexParamH264VUITimingInfo)) {
                    DEBUG_PRINT_ERROR("ERROR: Setting VUI timing info");
 return OMX_ErrorUnsupportedSetting;
 }
 break;
 }
 case OMX_QcomIndexParamPeakBitrate:
 {
                VALIDATE_OMX_PARAM_DATA(paramData, OMX_QCOM_VIDEO_PARAM_PEAK_BITRATE);
 if (!handle->venc_set_param(paramData,
 (OMX_INDEXTYPE) OMX_QcomIndexParamPeakBitrate)) {
                    DEBUG_PRINT_ERROR("ERROR: Setting peak bitrate");
 return OMX_ErrorUnsupportedSetting;
 }
 break;
 }
 case QOMX_IndexParamVideoInitialQp:
 {
                VALIDATE_OMX_PARAM_DATA(paramData, QOMX_EXTNINDEX_VIDEO_INITIALQP);
 if(!handle->venc_set_param(paramData,
 (OMX_INDEXTYPE)QOMX_IndexParamVideoInitialQp)) {
                    DEBUG_PRINT_ERROR("Request to Enable initial QP failed");
 return OMX_ErrorUnsupportedSetting;
 }
                memcpy(&m_sParamInitqp, paramData, sizeof(m_sParamInitqp));
 break;
 }
 case OMX_QcomIndexParamSetMVSearchrange:
 {
 if (!handle->venc_set_param(paramData,
 (OMX_INDEXTYPE) OMX_QcomIndexParamSetMVSearchrange)) {
                    DEBUG_PRINT_ERROR("ERROR: Setting Searchrange");
 return OMX_ErrorUnsupportedSetting;
 }
 break;
 }
 case OMX_QcomIndexParamVideoHybridHierpMode:
 {
                VALIDATE_OMX_PARAM_DATA(paramData, QOMX_EXTNINDEX_VIDEO_HYBRID_HP_MODE);
 if(!handle->venc_set_param(paramData,
 (OMX_INDEXTYPE)OMX_QcomIndexParamVideoHybridHierpMode)) {
                   DEBUG_PRINT_ERROR("Request to Enable Hybrid Hier-P failed");
 return OMX_ErrorUnsupportedSetting;
 }
 break;
 }
 case OMX_IndexParamVideoSliceFMO:
 default:
 {
                DEBUG_PRINT_ERROR("ERROR: Setparameter: unknown param %d", paramIndex);
                eRet = OMX_ErrorUnsupportedIndex;
 break;
 }
 }
 return eRet;
}
