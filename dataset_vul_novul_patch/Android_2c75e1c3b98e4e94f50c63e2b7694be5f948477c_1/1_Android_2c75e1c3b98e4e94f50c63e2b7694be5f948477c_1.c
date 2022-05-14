status_t OMXNodeInstance::useBuffer(
        OMX_U32 portIndex, const sp<IMemory> &params,
        OMX::buffer_id *buffer, OMX_U32 allottedSize) {
 if (params == NULL || buffer == NULL) {
        ALOGE("b/25884056");
 return BAD_VALUE;
 }

 Mutex::Autolock autoLock(mLock);
 if (allottedSize > params->size() || portIndex >= NELEM(mNumPortBuffers)) {
 return BAD_VALUE;
 }

 BufferMeta *buffer_meta;
 bool useBackup = mMetadataType[portIndex] != kMetadataBufferTypeInvalid;
    OMX_U8 *data = static_cast<OMX_U8 *>(params->pointer());
 if (useBackup) {
        data = new (std::nothrow) OMX_U8[allottedSize];
 if (data == NULL) {
 return NO_MEMORY;
 }
        memset(data, 0, allottedSize);

 if (allottedSize != params->size()) {
            CLOG_ERROR(useBuffer, BAD_VALUE, SIMPLE_BUFFER(portIndex, (size_t)allottedSize, data));
 delete[] data;
 return BAD_VALUE;
 }

        buffer_meta = new BufferMeta(

                 params, portIndex, false /* copyToOmx */, false /* copyFromOmx */, data);
     } else {
         buffer_meta = new BufferMeta(
                params, portIndex, false /* copyFromOmx */, false /* copyToOmx */, NULL);
     }
 
     OMX_BUFFERHEADERTYPE *header;

    OMX_ERRORTYPE err = OMX_UseBuffer(
            mHandle, &header, portIndex, buffer_meta,
            allottedSize, data);

 if (err != OMX_ErrorNone) {
        CLOG_ERROR(useBuffer, err, SIMPLE_BUFFER(
                portIndex, (size_t)allottedSize, data));

 delete buffer_meta;
        buffer_meta = NULL;

 *buffer = 0;

 return StatusFromOMXError(err);
 }

    CHECK_EQ(header->pAppPrivate, buffer_meta);

 *buffer = makeBufferID(header);

    addActiveBuffer(portIndex, *buffer);

    sp<GraphicBufferSource> bufferSource(getGraphicBufferSource());
 if (bufferSource != NULL && portIndex == kPortIndexInput) {
        bufferSource->addCodecBuffer(header);
 }

    CLOG_BUFFER(useBuffer, NEW_BUFFER_FMT(
 *buffer, portIndex, "%u(%zu)@%p", allottedSize, params->size(), params->pointer()));
 return OK;
}
