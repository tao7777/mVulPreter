status_t OMXCodec::allocateBuffersOnPort(OMX_U32 portIndex) {
 if (mNativeWindow != NULL && portIndex == kPortIndexOutput) {
 return allocateOutputBuffersFromNativeWindow();
 }

 if ((mFlags & kEnableGrallocUsageProtected) && portIndex == kPortIndexOutput) {
        ALOGE("protected output buffers must be stent to an ANativeWindow");
 return PERMISSION_DENIED;
 }

 status_t err = OK;
 if ((mFlags & kStoreMetaDataInVideoBuffers)
 && portIndex == kPortIndexInput) {
        err = mOMX->storeMetaDataInBuffers(mNode, kPortIndexInput, OMX_TRUE);
 if (err != OK) {
            ALOGE("Storing meta data in video buffers is not supported");
 return err;
 }
 }

    OMX_PARAM_PORTDEFINITIONTYPE def;
 InitOMXParams(&def);
    def.nPortIndex = portIndex;

    err = mOMX->getParameter(
            mNode, OMX_IndexParamPortDefinition, &def, sizeof(def));

 if (err != OK) {
 return err;
 }

    CODEC_LOGV("allocating %u buffers of size %u on %s port",
            def.nBufferCountActual, def.nBufferSize,
            portIndex == kPortIndexInput ? "input" : "output");

 if (def.nBufferSize != 0 && def.nBufferCountActual > SIZE_MAX / def.nBufferSize) {
 return BAD_VALUE;
 }
 size_t totalSize = def.nBufferCountActual * def.nBufferSize;
    mDealer[portIndex] = new MemoryDealer(totalSize, "OMXCodec");

 
     for (OMX_U32 i = 0; i < def.nBufferCountActual; ++i) {
         sp<IMemory> mem = mDealer[portIndex]->allocate(def.nBufferSize);
        if (mem == NULL || mem->pointer() == NULL) {
            return NO_MEMORY;
        }
 
         BufferInfo info;
         info.mData = NULL;
        info.mSize = def.nBufferSize;

        IOMX::buffer_id buffer;
 if (portIndex == kPortIndexInput
 && ((mQuirks & kRequiresAllocateBufferOnInputPorts)
 || (mFlags & kUseSecureInputBuffers))) {
 if (mOMXLivesLocally) {
                mem.clear();

                err = mOMX->allocateBuffer(
                        mNode, portIndex, def.nBufferSize, &buffer,
 &info.mData);
 } else {
                err = mOMX->allocateBufferWithBackup(
                        mNode, portIndex, mem, &buffer, mem->size());
 }
 } else if (portIndex == kPortIndexOutput
 && (mQuirks & kRequiresAllocateBufferOnOutputPorts)) {
 if (mOMXLivesLocally) {
                mem.clear();

                err = mOMX->allocateBuffer(
                        mNode, portIndex, def.nBufferSize, &buffer,
 &info.mData);
 } else {
                err = mOMX->allocateBufferWithBackup(
                        mNode, portIndex, mem, &buffer, mem->size());
 }
 } else {
            err = mOMX->useBuffer(mNode, portIndex, mem, &buffer, mem->size());
 }

 if (err != OK) {
            ALOGE("allocate_buffer_with_backup failed");
 return err;
 }

 if (mem != NULL) {
            info.mData = mem->pointer();
 }

        info.mBuffer = buffer;
        info.mStatus = OWNED_BY_US;
        info.mMem = mem;
        info.mMediaBuffer = NULL;

 if (portIndex == kPortIndexOutput) {
            LOG_ALWAYS_FATAL_IF((mOMXLivesLocally
 && (mQuirks & kRequiresAllocateBufferOnOutputPorts)
 && (mQuirks & kDefersOutputBufferAllocation)),
 "allocateBuffersOnPort cannot defer buffer allocation");

            info.mMediaBuffer = new MediaBuffer(info.mData, info.mSize);
            info.mMediaBuffer->setObserver(this);
 }

        mPortBuffers[portIndex].push(info);

        CODEC_LOGV("allocated buffer %u on %s port", buffer,
             portIndex == kPortIndexInput ? "input" : "output");
 }

 if (portIndex == kPortIndexOutput) {

        sp<MetaData> meta = mSource->getFormat();
 int32_t delay = 0;
 if (!meta->findInt32(kKeyEncoderDelay, &delay)) {
            delay = 0;
 }
 int32_t padding = 0;
 if (!meta->findInt32(kKeyEncoderPadding, &padding)) {
            padding = 0;
 }
 int32_t numchannels = 0;
 if (delay + padding) {
 if (mOutputFormat->findInt32(kKeyChannelCount, &numchannels)) {
 size_t frameSize = numchannels * sizeof(int16_t);
 if (mSkipCutBuffer != NULL) {
 size_t prevbuffersize = mSkipCutBuffer->size();
 if (prevbuffersize != 0) {
                        ALOGW("Replacing SkipCutBuffer holding %zu bytes", prevbuffersize);
 }
 }
                mSkipCutBuffer = new SkipCutBuffer(delay * frameSize, padding * frameSize);
 }
 }
 }


 if (portIndex == kPortIndexInput && (mFlags & kUseSecureInputBuffers)) {
 Vector<MediaBuffer *> buffers;
 for (size_t i = 0; i < def.nBufferCountActual; ++i) {
 const BufferInfo &info = mPortBuffers[kPortIndexInput].itemAt(i);

 MediaBuffer *mbuf = new MediaBuffer(info.mData, info.mSize);
            buffers.push(mbuf);
 }

 status_t err = mSource->setBuffers(buffers);

 if (err != OK) {
 for (size_t i = 0; i < def.nBufferCountActual; ++i) {
                buffers.editItemAt(i)->release();
 }
            buffers.clear();

            CODEC_LOGE(
 "Codec requested to use secure input buffers but "
 "upstream source didn't support that.");

 return err;
 }
 }

 return OK;
}
