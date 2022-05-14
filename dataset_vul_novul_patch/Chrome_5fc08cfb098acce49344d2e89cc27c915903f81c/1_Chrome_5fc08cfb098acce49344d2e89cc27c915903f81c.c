void ChromeDownloadDelegate::OnDownloadStarted(const std::string& filename,
                                               const std::string& mime_type) {
   JNIEnv* env = base::android::AttachCurrentThread();
   ScopedJavaLocalRef<jstring> jfilename = ConvertUTF8ToJavaString(
       env, filename);
  ScopedJavaLocalRef<jstring> jmime_type =
      ConvertUTF8ToJavaString(env, mime_type);
  Java_ChromeDownloadDelegate_onDownloadStarted(env, java_ref_, jfilename,
                                                jmime_type);
 }
