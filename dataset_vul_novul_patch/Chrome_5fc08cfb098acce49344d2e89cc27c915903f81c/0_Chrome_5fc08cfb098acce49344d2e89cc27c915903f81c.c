void ChromeDownloadDelegate::OnDownloadStarted(const std::string& filename,
void ChromeDownloadDelegate::OnDownloadStarted(const std::string& filename) {
   JNIEnv* env = base::android::AttachCurrentThread();
   ScopedJavaLocalRef<jstring> jfilename = ConvertUTF8ToJavaString(
       env, filename);
  Java_ChromeDownloadDelegate_onDownloadStarted(env, java_ref_, jfilename);
 }
