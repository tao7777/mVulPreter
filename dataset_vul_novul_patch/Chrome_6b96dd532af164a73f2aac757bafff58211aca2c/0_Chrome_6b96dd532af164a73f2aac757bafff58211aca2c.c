void ChromeWebContentsDelegateAndroid::AddNewContents(
    WebContents* source,
    WebContents* new_contents,
    WindowOpenDisposition disposition,
    const gfx::Rect& initial_rect,
    bool user_gesture,
    bool* was_blocked) {
  DCHECK_NE(disposition, SAVE_TO_DISK);
  DCHECK_NE(disposition, CURRENT_TAB);

  TabHelpers::AttachTabHelpers(new_contents);
 
   JNIEnv* env = AttachCurrentThread();
   ScopedJavaLocalRef<jobject> obj = GetJavaDelegate(env);
  bool handled = false;
   if (!obj.is_null()) {
     ScopedJavaLocalRef<jobject> jsource;
     if (source)
      jsource = source->GetJavaWebContents();
    ScopedJavaLocalRef<jobject> jnew_contents;
     if (new_contents)
       jnew_contents = new_contents->GetJavaWebContents();
 
    handled = Java_ChromeWebContentsDelegateAndroid_addNewContents(
        env,
        obj.obj(),
        jsource.obj(),
        jnew_contents.obj(),
        static_cast<jint>(disposition),
        NULL,
        user_gesture);
   }
 
   if (was_blocked)
    *was_blocked = !handled;
  if (!handled)
     delete new_contents;
 }
