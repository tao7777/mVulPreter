JNI_EXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
   base::android::InitVM(vm);
   if (!content::android::OnJNIOnLoadInit())
     return -1;
  content::SetContentMainDelegate(new content::ShellMainDelegate(true));
   return JNI_VERSION_1_4;
 }
