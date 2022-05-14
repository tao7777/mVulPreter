void CameraSource::releaseQueuedFrames() {

     List<sp<IMemory> >::iterator it;
     while (!mFramesReceived.empty()) {
         it = mFramesReceived.begin();
        // b/28466701
        adjustOutgoingANWBuffer(it->get());
         releaseRecordingFrame(*it);
         mFramesReceived.erase(it);
         ++mNumFramesDropped;
 }
}
