OMX_ERRORTYPE  omx_vdec::get_parameter(OMX_IN OMX_HANDLETYPE     hComp,
        OMX_IN OMX_INDEXTYPE paramIndex,
        OMX_INOUT OMX_PTR     paramData)
{
 (void) hComp;
    OMX_ERRORTYPE eRet = OMX_ErrorNone;

    DEBUG_PRINT_LOW("get_parameter:");
 if (m_state == OMX_StateInvalid) {
        DEBUG_PRINT_ERROR("Get Param in Invalid State");
 return OMX_ErrorInvalidState;
 }
 if (paramData == NULL) {
        DEBUG_PRINT_LOW("Get Param in Invalid paramData");
 return OMX_ErrorBadParameter;

     }
     switch ((unsigned long)paramIndex) {
         case OMX_IndexParamPortDefinition: {
                               VALIDATE_OMX_PARAM_DATA(paramData, OMX_PARAM_PORTDEFINITIONTYPE);
                                OMX_PARAM_PORTDEFINITIONTYPE *portDefn =
                                    (OMX_PARAM_PORTDEFINITIONTYPE *) paramData;
                                DEBUG_PRINT_LOW("get_parameter: OMX_IndexParamPortDefinition");
                               eRet = update_portdef(portDefn);
 if (eRet == OMX_ErrorNone)
                                   m_port_def = *portDefn;

                                break;
                            }
         case OMX_IndexParamVideoInit: {
                              VALIDATE_OMX_PARAM_DATA(paramData, OMX_PORT_PARAM_TYPE);
                               OMX_PORT_PARAM_TYPE *portParamType =
                                   (OMX_PORT_PARAM_TYPE *) paramData;
                               DEBUG_PRINT_LOW("get_parameter: OMX_IndexParamVideoInit");
 
                               portParamType->nVersion.nVersion = OMX_SPEC_VERSION;
                              portParamType->nSize = sizeof(OMX_PORT_PARAM_TYPE);
                               portParamType->nPorts           = 2;
                               portParamType->nStartPortNumber = 0;
                               break;
                           }
         case OMX_IndexParamVideoPortFormat: {
                                VALIDATE_OMX_PARAM_DATA(paramData, OMX_VIDEO_PARAM_PORTFORMATTYPE);
                                 OMX_VIDEO_PARAM_PORTFORMATTYPE *portFmt =
                                     (OMX_VIDEO_PARAM_PORTFORMATTYPE *)paramData;
                                 DEBUG_PRINT_LOW("get_parameter: OMX_IndexParamVideoPortFormat");
 
                                 portFmt->nVersion.nVersion = OMX_SPEC_VERSION;
                                portFmt->nSize             = sizeof(OMX_VIDEO_PARAM_PORTFORMATTYPE);
 
                                 if (0 == portFmt->nPortIndex) {
                                     if (0 == portFmt->nIndex) {
                                        portFmt->eColorFormat =  OMX_COLOR_FormatUnused;
                                        portFmt->eCompressionFormat = eCompressionFormat;
 } else {
                                        DEBUG_PRINT_ERROR("get_parameter: OMX_IndexParamVideoPortFormat:"\
 " NoMore compression formats");
                                        eRet =  OMX_ErrorNoMore;
 }
 } else if (1 == portFmt->nPortIndex) {
                                    portFmt->eCompressionFormat =  OMX_VIDEO_CodingUnused;

 bool useNonSurfaceMode = false;
#if defined(_ANDROID_) && !defined(FLEXYUV_SUPPORTED)
                                    useNonSurfaceMode = (m_enable_android_native_buffers == OMX_FALSE);
#endif
                                    portFmt->eColorFormat = useNonSurfaceMode ?
                                        getPreferredColorFormatNonSurfaceMode(portFmt->nIndex) :
                                        getPreferredColorFormatDefaultMode(portFmt->nIndex);

 if (portFmt->eColorFormat == OMX_COLOR_FormatMax ) {
                                        eRet = OMX_ErrorNoMore;
                                        DEBUG_PRINT_LOW("get_parameter: OMX_IndexParamVideoPortFormat:"\
 " NoMore Color formats");
 }
                                    DEBUG_PRINT_HIGH("returning color-format: 0x%x", portFmt->eColorFormat);
 } else {
                                    DEBUG_PRINT_ERROR("get_parameter: Bad port index %d",
 (int)portFmt->nPortIndex);
                                    eRet = OMX_ErrorBadPortIndex;
 }
 break;

                             }
                             /*Component should support this port definition*/
         case OMX_IndexParamAudioInit: {
                              VALIDATE_OMX_PARAM_DATA(paramData, OMX_PORT_PARAM_TYPE);
                               OMX_PORT_PARAM_TYPE *audioPortParamType =
                                   (OMX_PORT_PARAM_TYPE *) paramData;
                               DEBUG_PRINT_LOW("get_parameter: OMX_IndexParamAudioInit");
                               audioPortParamType->nVersion.nVersion = OMX_SPEC_VERSION;
                              audioPortParamType->nSize = sizeof(OMX_PORT_PARAM_TYPE);
                               audioPortParamType->nPorts           = 0;
                               audioPortParamType->nStartPortNumber = 0;
                               break;
                           }
                           /*Component should support this port definition*/
         case OMX_IndexParamImageInit: {
                              VALIDATE_OMX_PARAM_DATA(paramData, OMX_PORT_PARAM_TYPE);
                               OMX_PORT_PARAM_TYPE *imagePortParamType =
                                   (OMX_PORT_PARAM_TYPE *) paramData;
                               DEBUG_PRINT_LOW("get_parameter: OMX_IndexParamImageInit");
                               imagePortParamType->nVersion.nVersion = OMX_SPEC_VERSION;
                              imagePortParamType->nSize = sizeof(OMX_PORT_PARAM_TYPE);
                               imagePortParamType->nPorts           = 0;
                               imagePortParamType->nStartPortNumber = 0;
                               break;

 }
 /*Component should support this port definition*/
 case OMX_IndexParamOtherInit: {
                              DEBUG_PRINT_ERROR("get_parameter: OMX_IndexParamOtherInit %08x",
                                      paramIndex);
                              eRet =OMX_ErrorUnsupportedIndex;

                               break;
                           }
         case OMX_IndexParamStandardComponentRole: {
                                  VALIDATE_OMX_PARAM_DATA(paramData, OMX_PARAM_COMPONENTROLETYPE);
                                   OMX_PARAM_COMPONENTROLETYPE *comp_role;
                                   comp_role = (OMX_PARAM_COMPONENTROLETYPE *) paramData;
                                   comp_role->nVersion.nVersion = OMX_SPEC_VERSION;
                                  comp_role->nSize = sizeof(*comp_role);

                                  DEBUG_PRINT_LOW("Getparameter: OMX_IndexParamStandardComponentRole %d",
                                          paramIndex);
                                  strlcpy((char*)comp_role->cRole,(const char*)m_cRole,
                                          OMX_MAX_STRINGNAME_SIZE);
 break;

                               }
                               /* Added for parameter test */
         case OMX_IndexParamPriorityMgmt: {
                             VALIDATE_OMX_PARAM_DATA(paramData, OMX_PRIORITYMGMTTYPE);
                              OMX_PRIORITYMGMTTYPE *priorityMgmType =
                                  (OMX_PRIORITYMGMTTYPE *) paramData;
                              DEBUG_PRINT_LOW("get_parameter: OMX_IndexParamPriorityMgmt");
                              priorityMgmType->nVersion.nVersion = OMX_SPEC_VERSION;
                             priorityMgmType->nSize = sizeof(OMX_PRIORITYMGMTTYPE);
 
                              break;
                          }
                          /* Added for parameter test */
         case OMX_IndexParamCompBufferSupplier: {
                                   VALIDATE_OMX_PARAM_DATA(paramData, OMX_PARAM_BUFFERSUPPLIERTYPE);
                                    OMX_PARAM_BUFFERSUPPLIERTYPE *bufferSupplierType =
                                        (OMX_PARAM_BUFFERSUPPLIERTYPE*) paramData;
                                    DEBUG_PRINT_LOW("get_parameter: OMX_IndexParamCompBufferSupplier");
 
                                   bufferSupplierType->nSize = sizeof(OMX_PARAM_BUFFERSUPPLIERTYPE);
                                    bufferSupplierType->nVersion.nVersion = OMX_SPEC_VERSION;
                                    if (0 == bufferSupplierType->nPortIndex)
                                        bufferSupplierType->nPortIndex = OMX_BufferSupplyUnspecified;
 else if (1 == bufferSupplierType->nPortIndex)
                                       bufferSupplierType->nPortIndex = OMX_BufferSupplyUnspecified;
 else
                                       eRet = OMX_ErrorBadPortIndex;


 break;
 }
 case OMX_IndexParamVideoAvc: {
                             DEBUG_PRINT_LOW("get_parameter: OMX_IndexParamVideoAvc %08x",
                                     paramIndex);
 break;
 }
 case (OMX_INDEXTYPE)QOMX_IndexParamVideoMvc: {
                             DEBUG_PRINT_LOW("get_parameter: QOMX_IndexParamVideoMvc %08x",
                                     paramIndex);
 break;
 }
 case OMX_IndexParamVideoH263: {
                              DEBUG_PRINT_LOW("get_parameter: OMX_IndexParamVideoH263 %08x",
                                      paramIndex);
 break;
 }
 case OMX_IndexParamVideoMpeg4: {
                               DEBUG_PRINT_LOW("get_parameter: OMX_IndexParamVideoMpeg4 %08x",
                                       paramIndex);
 break;
 }
 case OMX_IndexParamVideoMpeg2: {
                               DEBUG_PRINT_LOW("get_parameter: OMX_IndexParamVideoMpeg2 %08x",
                                       paramIndex);

                                break;
                            }
         case OMX_IndexParamVideoProfileLevelQuerySupported: {
                                        VALIDATE_OMX_PARAM_DATA(paramData, OMX_VIDEO_PARAM_PROFILELEVELTYPE);
                                         DEBUG_PRINT_LOW("get_parameter: OMX_IndexParamVideoProfileLevelQuerySupported %08x", paramIndex);
                                         OMX_VIDEO_PARAM_PROFILELEVELTYPE *profileLevelType =
                                             (OMX_VIDEO_PARAM_PROFILELEVELTYPE *)paramData;
                                        eRet = get_supported_profile_level_for_1080p(profileLevelType);
 break;

                                     }
 #if defined (_ANDROID_HONEYCOMB_) || defined (_ANDROID_ICS_)
         case OMX_GoogleAndroidIndexGetAndroidNativeBufferUsage: {
                                        VALIDATE_OMX_PARAM_DATA(paramData, GetAndroidNativeBufferUsageParams);
                                         DEBUG_PRINT_LOW("get_parameter: OMX_GoogleAndroidIndexGetAndroidNativeBufferUsage");
                                         GetAndroidNativeBufferUsageParams* nativeBuffersUsage = (GetAndroidNativeBufferUsageParams *) paramData;
                                         if (nativeBuffersUsage->nPortIndex == OMX_CORE_OUTPUT_PORT_INDEX) {

 if (secure_mode && !secure_scaling_to_non_secure_opb) {
                                                nativeBuffersUsage->nUsage = (GRALLOC_USAGE_PRIVATE_MM_HEAP | GRALLOC_USAGE_PROTECTED |
                                                        GRALLOC_USAGE_PRIVATE_UNCACHED);
 } else {
                                                nativeBuffersUsage->nUsage =
 (GRALLOC_USAGE_PRIVATE_IOMMU_HEAP |
                                                     GRALLOC_USAGE_PRIVATE_UNCACHED);
 }
 } else {
                                            DEBUG_PRINT_HIGH("get_parameter: OMX_GoogleAndroidIndexGetAndroidNativeBufferUsage failed!");
                                            eRet = OMX_ErrorBadParameter;
 }
 }
 break;
#endif


 #ifdef FLEXYUV_SUPPORTED
         case OMX_QcomIndexFlexibleYUVDescription: {
                 DEBUG_PRINT_LOW("get_parameter: describeColorFormat");
                VALIDATE_OMX_PARAM_DATA(paramData, DescribeColorFormatParams);
                 eRet = describeColorFormat(paramData);
                 break;
             }
#endif

 default: {
                 DEBUG_PRINT_ERROR("get_parameter: unknown param %08x", paramIndex);
                 eRet =OMX_ErrorUnsupportedIndex;
 }

 }

    DEBUG_PRINT_LOW("get_parameter returning WxH(%d x %d) SxSH(%d x %d)",
            drv_ctx.video_resolution.frame_width,
            drv_ctx.video_resolution.frame_height,
            drv_ctx.video_resolution.stride,
            drv_ctx.video_resolution.scan_lines);

 return eRet;
}
