status_t OMXNodeInstance::createGraphicBufferSource(

         OMX_U32 portIndex, sp<IGraphicBufferConsumer> bufferConsumer, MetadataBufferType *type) {
     status_t err;
 
    // only allow graphic source on input port, when there are no allocated buffers yet
    if (portIndex != kPortIndexInput) {
        android_errorWriteLog(0x534e4554, "29422020");
        return BAD_VALUE;
    } else if (mNumPortBuffers[portIndex] > 0) {
        android_errorWriteLog(0x534e4554, "29422020");
        return INVALID_OPERATION;
    }

    const sp<GraphicBufferSource> surfaceCheck = getGraphicBufferSource();
     if (surfaceCheck != NULL) {
         if (portIndex < NELEM(mMetadataType) && type != NULL) {
             *type = mMetadataType[portIndex];
 }
 return ALREADY_EXISTS;
 }

 if (type != NULL) {
 *type = kMetadataBufferTypeANWBuffer;
 }
    err = storeMetaDataInBuffers_l(portIndex, OMX_TRUE, type);
 if (err != OK) {
 return err;
 }

    OMX_PARAM_PORTDEFINITIONTYPE def;
 InitOMXParams(&def);
    def.nPortIndex = portIndex;
    OMX_ERRORTYPE oerr = OMX_GetParameter(
            mHandle, OMX_IndexParamPortDefinition, &def);
 if (oerr != OMX_ErrorNone) {
        OMX_INDEXTYPE index = OMX_IndexParamPortDefinition;
        CLOG_ERROR(getParameter, oerr, "%s(%#x): %s:%u",
                asString(index), index, portString(portIndex), portIndex);
 return UNKNOWN_ERROR;
 }

 if (def.format.video.eColorFormat != OMX_COLOR_FormatAndroidOpaque) {
        CLOGW("createInputSurface requires COLOR_FormatSurface "
 "(AndroidOpaque) color format instead of %s(%#x)",
                asString(def.format.video.eColorFormat), def.format.video.eColorFormat);
 return INVALID_OPERATION;
 }

 uint32_t usageBits;
    oerr = OMX_GetParameter(
            mHandle, (OMX_INDEXTYPE)OMX_IndexParamConsumerUsageBits, &usageBits);
 if (oerr != OMX_ErrorNone) {
        usageBits = 0;
 }

    sp<GraphicBufferSource> bufferSource = new GraphicBufferSource(this,
            def.format.video.nFrameWidth,
            def.format.video.nFrameHeight,
            def.nBufferCountActual,
            usageBits,
            bufferConsumer);

 if ((err = bufferSource->initCheck()) != OK) {
 return err;
 }
    setGraphicBufferSource(bufferSource);

 return OK;
}
