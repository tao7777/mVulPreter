status_t OMXNodeInstance::useGraphicBuffer2_l(
        OMX_U32 portIndex, const sp<GraphicBuffer>& graphicBuffer,
        OMX::buffer_id *buffer) {

    OMX_PARAM_PORTDEFINITIONTYPE def;
 InitOMXParams(&def);
    def.nPortIndex = portIndex;
    OMX_ERRORTYPE err = OMX_GetParameter(mHandle, OMX_IndexParamPortDefinition, &def);
 if (err != OMX_ErrorNone) {
        OMX_INDEXTYPE index = OMX_IndexParamPortDefinition;
        CLOG_ERROR(getParameter, err, "%s(%#x): %s:%u",
                asString(index), index, portString(portIndex), portIndex);

         return UNKNOWN_ERROR;
     }
 
    BufferMeta *bufferMeta = new BufferMeta(graphicBuffer, portIndex);
 
     OMX_BUFFERHEADERTYPE *header = NULL;
     OMX_U8* bufferHandle = const_cast<OMX_U8*>(
 reinterpret_cast<const OMX_U8*>(graphicBuffer->handle));

    err = OMX_UseBuffer(
            mHandle,
 &header,
            portIndex,
            bufferMeta,
            def.nBufferSize,
            bufferHandle);

 if (err != OMX_ErrorNone) {
        CLOG_ERROR(useBuffer, err, BUFFER_FMT(portIndex, "%u@%p", def.nBufferSize, bufferHandle));
 delete bufferMeta;
        bufferMeta = NULL;
 *buffer = 0;
 return StatusFromOMXError(err);
 }

    CHECK_EQ(header->pBuffer, bufferHandle);
    CHECK_EQ(header->pAppPrivate, bufferMeta);

 *buffer = makeBufferID(header);

    addActiveBuffer(portIndex, *buffer);
    CLOG_BUFFER(useGraphicBuffer2, NEW_BUFFER_FMT(
 *buffer, portIndex, "%u@%p", def.nBufferSize, bufferHandle));
 return OK;
}
