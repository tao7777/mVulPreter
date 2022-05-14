 MediaMetadataRetriever::MediaMetadataRetriever()
 {
     ALOGV("constructor");
    const sp<IMediaPlayerService>& service(getService());
     if (service == 0) {
         ALOGE("failed to obtain MediaMetadataRetrieverService");
         return;
 }
    sp<IMediaMetadataRetriever> retriever(service->createMetadataRetriever());
 if (retriever == 0) {
        ALOGE("failed to create IMediaMetadataRetriever object from server");
 }
    mRetriever = retriever;
}
