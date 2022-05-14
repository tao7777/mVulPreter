status_t StreamingProcessor::processRecordingFrame() {
    ATRACE_CALL();
 status_t res;
    sp<Camera2Heap> recordingHeap;
 size_t heapIdx = 0;
 nsecs_t timestamp;

    sp<Camera2Client> client = mClient.promote();
 if (client == 0) {
 BufferItem imgBuffer;
        res = mRecordingConsumer->acquireBuffer(&imgBuffer, 0);
 if (res != OK) {
 if (res != BufferItemConsumer::NO_BUFFER_AVAILABLE) {
                ALOGE("%s: Camera %d: Can't acquire recording buffer: %s (%d)",
                        __FUNCTION__, mId, strerror(-res), res);
 }
 return res;
 }
        mRecordingConsumer->releaseBuffer(imgBuffer);
 return OK;
 }

 {
 /* acquire SharedParameters before mMutex so we don't dead lock
            with Camera2Client code calling into StreamingProcessor */
 SharedParameters::Lock l(client->getParameters());
 Mutex::Autolock m(mMutex);
 BufferItem imgBuffer;
        res = mRecordingConsumer->acquireBuffer(&imgBuffer, 0);
 if (res != OK) {
 if (res != BufferItemConsumer::NO_BUFFER_AVAILABLE) {
                ALOGE("%s: Camera %d: Can't acquire recording buffer: %s (%d)",
                        __FUNCTION__, mId, strerror(-res), res);
 }
 return res;
 }
        timestamp = imgBuffer.mTimestamp;

        mRecordingFrameCount++;
        ALOGVV("OnRecordingFrame: Frame %d", mRecordingFrameCount);

 if (l.mParameters.state != Parameters::RECORD &&
                l.mParameters.state != Parameters::VIDEO_SNAPSHOT) {
            ALOGV("%s: Camera %d: Discarding recording image buffers "
 "received after recording done", __FUNCTION__,
                    mId);
            mRecordingConsumer->releaseBuffer(imgBuffer);
 return INVALID_OPERATION;
 }

 if (mRecordingHeap == 0) {
 size_t payloadSize = sizeof(VideoNativeMetadata);
            ALOGV("%s: Camera %d: Creating recording heap with %zu buffers of "
 "size %zu bytes", __FUNCTION__, mId,
                    mRecordingHeapCount, payloadSize);

            mRecordingHeap = new Camera2Heap(payloadSize, mRecordingHeapCount,
 "Camera2Client::RecordingHeap");
 if (mRecordingHeap->mHeap->getSize() == 0) {
                ALOGE("%s: Camera %d: Unable to allocate memory for recording",
                        __FUNCTION__, mId);
                mRecordingConsumer->releaseBuffer(imgBuffer);
 return NO_MEMORY;
 }
 for (size_t i = 0; i < mRecordingBuffers.size(); i++) {
 if (mRecordingBuffers[i].mBuf !=
 BufferItemConsumer::INVALID_BUFFER_SLOT) {
                    ALOGE("%s: Camera %d: Non-empty recording buffers list!",
                            __FUNCTION__, mId);
 }
 }
            mRecordingBuffers.clear();
            mRecordingBuffers.setCapacity(mRecordingHeapCount);
            mRecordingBuffers.insertAt(0, mRecordingHeapCount);

            mRecordingHeapHead = 0;
            mRecordingHeapFree = mRecordingHeapCount;
 }

 if (mRecordingHeapFree == 0) {
            ALOGE("%s: Camera %d: No free recording buffers, dropping frame",
                    __FUNCTION__, mId);
            mRecordingConsumer->releaseBuffer(imgBuffer);
 return NO_MEMORY;
 }

        heapIdx = mRecordingHeapHead;
        mRecordingHeapHead = (mRecordingHeapHead + 1) % mRecordingHeapCount;
        mRecordingHeapFree--;

        ALOGVV("%s: Camera %d: Timestamp %lld",
                __FUNCTION__, mId, timestamp);

 ssize_t offset;
 size_t size;
        sp<IMemoryHeap> heap =
                mRecordingHeap->mBuffers[heapIdx]->getMemory(&offset,
 &size);

 VideoNativeMetadata *payload = reinterpret_cast<VideoNativeMetadata*>(

             (uint8_t*)heap->getBase() + offset);
         payload->eType = kMetadataBufferTypeANWBuffer;
         payload->pBuffer = imgBuffer.mGraphicBuffer->getNativeBuffer();
        // b/28466701
        payload->pBuffer = (ANativeWindowBuffer*)((uint8_t*)payload->pBuffer -
                ICameraRecordingProxy::getCommonBaseAddress());
         payload->nFenceFd = -1;
 
         ALOGVV("%s: Camera %d: Sending out ANWBuffer %p",
                __FUNCTION__, mId, payload->pBuffer);

        mRecordingBuffers.replaceAt(imgBuffer, heapIdx);
        recordingHeap = mRecordingHeap;
 }

 Camera2Client::SharedCameraCallbacks::Lock l(client->mSharedCameraCallbacks);
 if (l.mRemoteCallback != 0) {
        l.mRemoteCallback->dataCallbackTimestamp(timestamp,
                CAMERA_MSG_VIDEO_FRAME,
                recordingHeap->mBuffers[heapIdx]);
 } else {
        ALOGW("%s: Camera %d: Remote callback gone", __FUNCTION__, mId);
 }

 return OK;
}
