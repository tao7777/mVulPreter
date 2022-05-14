void CameraSource::signalBufferReturned(MediaBuffer *buffer) {
    ALOGV("signalBufferReturned: %p", buffer->data());
 Mutex::Autolock autoLock(mLock);

     for (List<sp<IMemory> >::iterator it = mFramesBeingEncoded.begin();
          it != mFramesBeingEncoded.end(); ++it) {
         if ((*it)->pointer() ==  buffer->data()) {
            // b/28466701
            adjustOutgoingANWBuffer(it->get());

             releaseOneRecordingFrame((*it));
             mFramesBeingEncoded.erase(it);
             ++mNumFramesEncoded;
            buffer->setObserver(0);
            buffer->release();
            mFrameCompleteCondition.signal();
 return;
 }
 }
    CHECK(!"signalBufferReturned: bogus buffer");
}
