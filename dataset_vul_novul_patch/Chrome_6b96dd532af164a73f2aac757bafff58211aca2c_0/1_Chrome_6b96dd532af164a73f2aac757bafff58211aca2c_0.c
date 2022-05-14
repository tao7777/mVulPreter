void WebContentsAndroid::OpenURL(JNIEnv* env,
                                 jobject obj,
                                 jstring url,
                                 jboolean user_gesture,
                                 jboolean is_renderer_initiated) {
  GURL gurl(base::android::ConvertJavaStringToUTF8(env, url));
  OpenURLParams open_params(gurl,
                            Referrer(),
                            CURRENT_TAB,
                            ui::PAGE_TRANSITION_LINK,
                            is_renderer_initiated);
  open_params.user_gesture = user_gesture;
  web_contents_->OpenURL(open_params);
}
