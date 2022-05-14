MediaRecorder::MediaRecorder(const String16& opPackageName) : mSurfaceMediaSource(NULL)

 {
     ALOGV("constructor");
 
    const sp<IMediaPlayerService>& service(getMediaPlayerService());
     if (service != NULL) {
         mMediaRecorder = service->createMediaRecorder(opPackageName);
     }
 if (mMediaRecorder != NULL) {
        mCurrentState = MEDIA_RECORDER_IDLE;
 }


    doCleanUp();
}
