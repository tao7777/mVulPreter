status_t MediaPlayerService::Client::setNextPlayer(const sp<IMediaPlayer>& player) {

     ALOGV("setNextPlayer");
     Mutex::Autolock l(mLock);
     sp<Client> c = static_cast<Client*>(player.get());
     mNextClient = c;
 
     if (c != NULL) {
 if (mAudioOutput != NULL) {
            mAudioOutput->setNextOutput(c->mAudioOutput);
 } else if ((mPlayer != NULL) && !mPlayer->hardwareOutput()) {
            ALOGE("no current audio output");
 }

 if ((mPlayer != NULL) && (mNextClient->getPlayer() != NULL)) {
            mPlayer->setNextPlayer(mNextClient->getPlayer());
 }
 }

 return OK;
}
