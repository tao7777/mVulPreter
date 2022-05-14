void ExternalFeedbackReporterAndroid::ReportExternalFeedback(
//// static
void DistillerUIHandleAndroid::ReportExternalFeedback(
     content::WebContents* web_contents,
     const GURL& url,
     const bool good) {
   if (!web_contents)
     return;
 
   JNIEnv* env = base::android::AttachCurrentThread();
   ScopedJavaLocalRef<jstring> jurl = base::android::ConvertUTF8ToJavaString(
       env, url_utils::GetOriginalUrlFromDistillerUrl(url).spec());
 
  Java_DomDistillerUIUtils_reportFeedbackWithWebContents(
      env, web_contents->GetJavaWebContents().obj(), jurl.obj(), good);
}

//// static
void DistillerUIHandleAndroid::OpenSettings(
    content::WebContents* web_contents) {
  JNIEnv* env = base::android::AttachCurrentThread();
  Java_DomDistillerUIUtils_openSettings(env,
      web_contents->GetJavaWebContents().obj());
 }
