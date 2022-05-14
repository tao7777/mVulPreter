void StreamingProcessor::releaseRecordingFrame(const sp<IMemory>& mem) {
    ATRACE_CALL();
 status_t res;

 Mutex::Autolock m(mMutex);
 ssize_t offset;
 size_t size;
    sp<IMemoryHeap> heap = mem->getMemory(&offset, &size);
 if (heap->getHeapID() != mRecordingHeap->mHeap->getHeapID()) {
        ALOGW("%s: Camera %d: Mismatched heap ID, ignoring release "
 "(got %x, expected %x)", __FUNCTION__, mId,
                heap->getHeapID(), mRecordingHeap->mHeap->getHeapID());
 return;
 }

 VideoNativeMetadata *payload = reinterpret_cast<VideoNativeMetadata*>(
 (uint8_t*)heap->getBase() + offset);

 if (payload->eType != kMetadataBufferTypeANWBuffer) {
        ALOGE("%s: Camera %d: Recording frame type invalid (got %x, expected %x)",
                __FUNCTION__, mId, payload->eType,
                kMetadataBufferTypeANWBuffer);

         return;
     }
 
    // b/28466701
    payload->pBuffer = (ANativeWindowBuffer*)(((uint8_t*)payload->pBuffer) +
            ICameraRecordingProxy::getCommonBaseAddress());

     size_t itemIndex;
     for (itemIndex = 0; itemIndex < mRecordingBuffers.size(); itemIndex++) {
 const BufferItem item = mRecordingBuffers[itemIndex];
 if (item.mBuf != BufferItemConsumer::INVALID_BUFFER_SLOT &&
                item.mGraphicBuffer->getNativeBuffer() == payload->pBuffer) {
 break;
 }
 }

 if (itemIndex == mRecordingBuffers.size()) {
        ALOGE("%s: Camera %d: Can't find returned ANW Buffer %p in list of "
 "outstanding buffers", __FUNCTION__, mId,
                payload->pBuffer);
 return;
 }

    ALOGVV("%s: Camera %d: Freeing returned ANW buffer %p index %d", __FUNCTION__,
            mId, payload->pBuffer, itemIndex);

    res = mRecordingConsumer->releaseBuffer(mRecordingBuffers[itemIndex]);
 if (res != OK) {
        ALOGE("%s: Camera %d: Unable to free recording frame "
 "(Returned ANW buffer: %p): %s (%d)", __FUNCTION__,
                mId, payload->pBuffer, strerror(-res), res);
 return;
 }
    mRecordingBuffers.replaceAt(itemIndex);

    mRecordingHeapFree++;
    ALOGV_IF(mRecordingHeapFree == mRecordingHeapCount,
 "%s: Camera %d: All %d recording buffers returned",
            __FUNCTION__, mId, mRecordingHeapCount);
}
