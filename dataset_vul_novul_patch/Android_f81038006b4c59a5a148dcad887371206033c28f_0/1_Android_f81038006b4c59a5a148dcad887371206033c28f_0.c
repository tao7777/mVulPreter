 status_t NuPlayer::GenericSource::setBuffers(
         bool audio, Vector<MediaBuffer *> &buffers) {
    if (mIsSecure && !audio) {
         return mVideoTrack.mSource->setBuffers(buffers);
     }
     return INVALID_OPERATION;
}
