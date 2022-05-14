status_t MediaPlayer::setDataSource(const sp<IStreamSource> &source)

 {
     ALOGV("setDataSource");
     status_t err = UNKNOWN_ERROR;
    const sp<IMediaPlayerService> service(getMediaPlayerService());
     if (service != 0) {
         sp<IMediaPlayer> player(service->create(this, mAudioSessionId));
         if ((NO_ERROR != doSetRetransmitEndpoint(player)) ||
 (NO_ERROR != player->setDataSource(source))) {
            player.clear();
 }
        err = attachNewPlayer(player);
 }
 return err;
}
