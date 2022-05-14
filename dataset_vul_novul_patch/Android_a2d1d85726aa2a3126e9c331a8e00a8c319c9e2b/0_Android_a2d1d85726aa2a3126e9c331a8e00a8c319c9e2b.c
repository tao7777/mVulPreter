ssize_t NuPlayer::NuPlayerStreamListener::read(
 void *data, size_t size, sp<AMessage> *extra) {
    CHECK_GT(size, 0u);

    extra->clear();

 Mutex::Autolock autoLock(mLock);

 if (mEOS) {
 return 0;
 }

 if (mQueue.empty()) {
        mSendDataNotification = true;

 return -EWOULDBLOCK;
 }

 QueueEntry *entry = &*mQueue.begin();

 if (entry->mIsCommand) {
 switch (entry->mCommand) {
 case EOS:
 {
                mQueue.erase(mQueue.begin());
                entry = NULL;

                mEOS = true;
 return 0;
 }

 case DISCONTINUITY:
 {
 *extra = entry->mExtra;

                mQueue.erase(mQueue.begin());
                entry = NULL;

 return INFO_DISCONTINUITY;
 }

 default:
                TRESPASS();
 break;
 }
 }

 size_t copy = entry->mSize;
 if (copy > size) {

         copy = size;
     }
 
    if (entry->mIndex >= mBuffers.size()) {
        return ERROR_MALFORMED;
    }

    sp<IMemory> mem = mBuffers.editItemAt(entry->mIndex);
    if (mem == NULL || mem->size() < copy || mem->size() - copy < entry->mOffset) {
        return ERROR_MALFORMED;
    }

     memcpy(data,
           (const uint8_t *)mem->pointer()
             + entry->mOffset,
            copy);
 
    entry->mOffset += copy;
    entry->mSize -= copy;

 if (entry->mSize == 0) {
        mSource->onBufferAvailable(entry->mIndex);
        mQueue.erase(mQueue.begin());
        entry = NULL;
 }

 return copy;
}
