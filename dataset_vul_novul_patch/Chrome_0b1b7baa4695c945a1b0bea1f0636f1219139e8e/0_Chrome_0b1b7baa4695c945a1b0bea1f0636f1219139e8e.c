void DownloadUIAdapterDelegate::OpenItem(const OfflineItem& item,
                                          int64_t offline_id) {
   JNIEnv* env = AttachCurrentThread();
   Java_OfflinePageDownloadBridge_openItem(
      env, ConvertUTF8ToJavaString(env, item.page_url.spec()), offline_id,
      offline_pages::ShouldOfflinePagesInDownloadHomeOpenInCct());
 }
