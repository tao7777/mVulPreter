status_t OMXNodeInstance::useGraphicBuffer(
        OMX_U32 portIndex, const sp<GraphicBuffer>& graphicBuffer,
        OMX::buffer_id *buffer) {
 Mutex::Autolock autoLock(mLock);

    OMX_INDEXTYPE index;
 if (OMX_GetExtensionIndex(
            mHandle,
 const_cast<OMX_STRING>("OMX.google.android.index.useAndroidNativeBuffer2"),
 &index) == OMX_ErrorNone) {
 return useGraphicBuffer2_l(portIndex, graphicBuffer, buffer);
 }

    OMX_STRING name = const_cast<OMX_STRING>(
 "OMX.google.android.index.useAndroidNativeBuffer");
    OMX_ERRORTYPE err = OMX_GetExtensionIndex(mHandle, name, &index);
 if (err != OMX_ErrorNone) {
        CLOG_ERROR(getExtensionIndex, err, "%s", name);

         return StatusFromOMXError(err);
     }
 
    BufferMeta *bufferMeta = new BufferMeta(graphicBuffer);
 
     OMX_BUFFERHEADERTYPE *header;
 
    OMX_VERSIONTYPE ver;
    ver.s.nVersionMajor = 1;
    ver.s.nVersionMinor = 0;
    ver.s.nRevision = 0;
    ver.s.nStep = 0;
 UseAndroidNativeBufferParams params = {
 sizeof(UseAndroidNativeBufferParams), ver, portIndex, bufferMeta,
 &header, graphicBuffer,
 };

    err = OMX_SetParameter(mHandle, index, &params);

 if (err != OMX_ErrorNone) {
        CLOG_ERROR(setParameter, err, "%s(%#x): %s:%u meta=%p GB=%p", name, index,
                portString(portIndex), portIndex, bufferMeta, graphicBuffer->handle);

 delete bufferMeta;
        bufferMeta = NULL;

 *buffer = 0;

 return StatusFromOMXError(err);
 }

    CHECK_EQ(header->pAppPrivate, bufferMeta);

 *buffer = makeBufferID(header);

    addActiveBuffer(portIndex, *buffer);
    CLOG_BUFFER(useGraphicBuffer, NEW_BUFFER_FMT(
 *buffer, portIndex, "GB=%p", graphicBuffer->handle));
 return OK;
}
