void ExternalFeedbackReporterAndroid::ReportExternalFeedback(
//// static
     content::WebContents* web_contents,
     const GURL& url,
     const bool good) {
   if (!web_contents)
     return;
  WindowAndroidHelper* helper =
      content::WebContentsUserData<WindowAndroidHelper>::FromWebContents(
          web_contents);
  DCHECK(helper);
  ui::WindowAndroid* window = helper->GetWindowAndroid();
  DCHECK(window);
 
   JNIEnv* env = base::android::AttachCurrentThread();
   ScopedJavaLocalRef<jstring> jurl = base::android::ConvertUTF8ToJavaString(
       env, url_utils::GetOriginalUrlFromDistillerUrl(url).spec());
 
  Java_DomDistillerFeedbackReporter_reportFeedbackWithWindow(
      env, window->GetJavaObject().obj(), jurl.obj(), good);
//// static
 }
