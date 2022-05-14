void ChromeDownloadDelegate::RequestHTTPGetDownload(
    const std::string& url,
    const std::string& user_agent,
    const std::string& content_disposition,
    const std::string& mime_type,
    const std::string& cookie,
    const std::string& referer,
    const base::string16& file_name,
    int64_t content_length,
    bool has_user_gesture,
    bool must_download) {
  JNIEnv* env = base::android::AttachCurrentThread();
  ScopedJavaLocalRef<jstring> jurl =
      ConvertUTF8ToJavaString(env, url);
  ScopedJavaLocalRef<jstring> juser_agent =
      ConvertUTF8ToJavaString(env, user_agent);
  ScopedJavaLocalRef<jstring> jcontent_disposition =
      ConvertUTF8ToJavaString(env, content_disposition);
  ScopedJavaLocalRef<jstring> jmime_type =
      ConvertUTF8ToJavaString(env, mime_type);
  ScopedJavaLocalRef<jstring> jcookie =
      ConvertUTF8ToJavaString(env, cookie);
  ScopedJavaLocalRef<jstring> jreferer =
      ConvertUTF8ToJavaString(env, referer);
  ScopedJavaLocalRef<jstring> jfilename =
      base::android::ConvertUTF16ToJavaString(env, file_name);
  Java_ChromeDownloadDelegate_requestHttpGetDownload(
      env, java_ref_, jurl, juser_agent, jcontent_disposition, jmime_type,
      jcookie, jreferer, has_user_gesture, jfilename, content_length,
      must_download);
}
