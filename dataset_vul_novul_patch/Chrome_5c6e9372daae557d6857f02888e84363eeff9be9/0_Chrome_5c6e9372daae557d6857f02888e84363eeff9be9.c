 void MostVisitedSitesBridge::JavaObserver::OnIconMadeAvailable(
     const GURL& site_url) {
   JNIEnv* env = AttachCurrentThread();
  Java_Observer_onIconMadeAvailable(
       env, observer_, ConvertUTF8ToJavaString(env, site_url.spec()));
 }
