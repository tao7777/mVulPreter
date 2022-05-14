 status_t OMXNodeInstance::storeMetaDataInBuffers_l(
         OMX_U32 portIndex, OMX_BOOL enable, MetadataBufferType *type) {
     if (portIndex != kPortIndexInput && portIndex != kPortIndexOutput) {
         android_errorWriteLog(0x534e4554, "26324358");
         if (type != NULL) {
 *type = kMetadataBufferTypeInvalid;
 }
 return BAD_VALUE;
 }

    OMX_INDEXTYPE index;
    OMX_STRING name = const_cast<OMX_STRING>(
 "OMX.google.android.index.storeMetaDataInBuffers");

    OMX_STRING nativeBufferName = const_cast<OMX_STRING>(
 "OMX.google.android.index.storeANWBufferInMetadata");
 MetadataBufferType negotiatedType;
 MetadataBufferType requestedType = type != NULL ? *type : kMetadataBufferTypeANWBuffer;

 StoreMetaDataInBuffersParams params;
 InitOMXParams(&params);
    params.nPortIndex = portIndex;
    params.bStoreMetaData = enable;

    OMX_ERRORTYPE err =
        requestedType == kMetadataBufferTypeANWBuffer
 ? OMX_GetExtensionIndex(mHandle, nativeBufferName, &index)
 : OMX_ErrorUnsupportedIndex;
    OMX_ERRORTYPE xerr = err;
 if (err == OMX_ErrorNone) {
        err = OMX_SetParameter(mHandle, index, &params);
 if (err == OMX_ErrorNone) {
            name = nativeBufferName; // set name for debugging
            negotiatedType = requestedType;
 }
 }
 if (err != OMX_ErrorNone) {
        err = OMX_GetExtensionIndex(mHandle, name, &index);
        xerr = err;
 if (err == OMX_ErrorNone) {
            negotiatedType =
                requestedType == kMetadataBufferTypeANWBuffer
 ? kMetadataBufferTypeGrallocSource : requestedType;
            err = OMX_SetParameter(mHandle, index, &params);
 }
 }

 if (err != OMX_ErrorNone) {
 if (err == OMX_ErrorUnsupportedIndex && portIndex == kPortIndexOutput) {
            CLOGW("component does not support metadata mode; using fallback");
 } else if (xerr != OMX_ErrorNone) {
            CLOG_ERROR(getExtensionIndex, xerr, "%s", name);
 } else {
            CLOG_ERROR(setParameter, err, "%s(%#x): %s:%u en=%d type=%d", name, index,
                    portString(portIndex), portIndex, enable, negotiatedType);
 }
        negotiatedType = mMetadataType[portIndex];
 } else {
 if (!enable) {
            negotiatedType = kMetadataBufferTypeInvalid;
 }
        mMetadataType[portIndex] = negotiatedType;
 }
    CLOG_CONFIG(storeMetaDataInBuffers, "%s:%u %srequested %s:%d negotiated %s:%d",
            portString(portIndex), portIndex, enable ? "" : "UN",
            asString(requestedType), requestedType, asString(negotiatedType), negotiatedType);

 if (type != NULL) {
 *type = negotiatedType;
 }

 return StatusFromOMXError(err);
}
