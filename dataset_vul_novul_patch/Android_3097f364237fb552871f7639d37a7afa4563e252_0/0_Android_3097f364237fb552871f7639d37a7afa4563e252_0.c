const sp<IMediaPlayerService>& MediaMetadataRetriever::getService()
const sp<IMediaPlayerService> MediaMetadataRetriever::getService()
 {
     Mutex::Autolock lock(sServiceLock);
     if (sService == 0) {
        sp<IServiceManager> sm = defaultServiceManager();
        sp<IBinder> binder;
 do {
            binder = sm->getService(String16("media.player"));
 if (binder != 0) {
 break;
 }
            ALOGW("MediaPlayerService not published, waiting...");
            usleep(500000); // 0.5 s
 } while (true);
 if (sDeathNotifier == NULL) {
            sDeathNotifier = new DeathNotifier();
 }
        binder->linkToDeath(sDeathNotifier);
        sService = interface_cast<IMediaPlayerService>(binder);
 }
    ALOGE_IF(sService == 0, "no MediaPlayerService!?");
 return sService;
}
