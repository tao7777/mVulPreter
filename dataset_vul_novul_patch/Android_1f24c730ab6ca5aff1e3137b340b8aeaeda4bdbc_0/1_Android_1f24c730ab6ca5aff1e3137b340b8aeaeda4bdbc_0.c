void CameraSource::releaseQueuedFrames() {

     List<sp<IMemory> >::iterator it;
     while (!mFramesReceived.empty()) {
         it = mFramesReceived.begin();
         releaseRecordingFrame(*it);
         mFramesReceived.erase(it);
         ++mNumFramesDropped;
 }
}
