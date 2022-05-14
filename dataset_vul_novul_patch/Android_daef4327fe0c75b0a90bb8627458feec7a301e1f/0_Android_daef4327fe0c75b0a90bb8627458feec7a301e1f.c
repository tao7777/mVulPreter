sp<IMemory> MetadataRetrieverClient::getFrameAtTime(int64_t timeUs, int option)
{
    ALOGV("getFrameAtTime: time(%lld us) option(%d)", timeUs, option);
 Mutex::Autolock lock(mLock);
 Mutex::Autolock glock(sLock);
    mThumbnail.clear();
 if (mRetriever == NULL) {
        ALOGE("retriever is not initialized");
 return NULL;
 }
 VideoFrame *frame = mRetriever->getFrameAtTime(timeUs, option);
 if (frame == NULL) {
        ALOGE("failed to capture a video frame");
 return NULL;
 }
 size_t size = sizeof(VideoFrame) + frame->mSize;
    sp<MemoryHeapBase> heap = new MemoryHeapBase(size, 0, "MetadataRetrieverClient");
 if (heap == NULL) {
        ALOGE("failed to create MemoryDealer");
 delete frame;
 return NULL;
 }
    mThumbnail = new MemoryBase(heap, 0, size);
 if (mThumbnail == NULL) {
        ALOGE("not enough memory for VideoFrame size=%u", size);
 delete frame;
 return NULL;
 }
 VideoFrame *frameCopy = static_cast<VideoFrame *>(mThumbnail->pointer());
    frameCopy->mWidth = frame->mWidth;
    frameCopy->mHeight = frame->mHeight;
    frameCopy->mDisplayWidth = frame->mDisplayWidth;
    frameCopy->mDisplayHeight = frame->mDisplayHeight;
    frameCopy->mSize = frame->mSize;
    frameCopy->mRotationAngle = frame->mRotationAngle;

     ALOGV("rotation: %d", frameCopy->mRotationAngle);
     frameCopy->mData = (uint8_t *)frameCopy + sizeof(VideoFrame);
     memcpy(frameCopy->mData, frame->mData, frame->mSize);
    frameCopy->mData = 0;
     delete frame;  // Fix memory leakage
     return mThumbnail;
 }
