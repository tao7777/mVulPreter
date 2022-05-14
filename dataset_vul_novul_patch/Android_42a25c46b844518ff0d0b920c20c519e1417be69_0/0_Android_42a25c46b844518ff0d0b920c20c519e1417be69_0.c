status_t MediaPlayer::setDataSource(int fd, int64_t offset, int64_t length)

 {
     ALOGV("setDataSource(%d, %" PRId64 ", %" PRId64 ")", fd, offset, length);
     status_t err = UNKNOWN_ERROR;
    const sp<IMediaPlayerService> service(getMediaPlayerService());
     if (service != 0) {
         sp<IMediaPlayer> player(service->create(this, mAudioSessionId));
         if ((NO_ERROR != doSetRetransmitEndpoint(player)) ||
 (NO_ERROR != player->setDataSource(fd, offset, length))) {
            player.clear();
 }
        err = attachNewPlayer(player);
 }
 return err;
}
