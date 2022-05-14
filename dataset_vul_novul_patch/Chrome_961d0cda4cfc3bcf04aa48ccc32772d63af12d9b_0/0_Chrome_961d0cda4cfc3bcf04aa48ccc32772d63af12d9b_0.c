 PasswordGenerationDialogViewAndroid::PasswordGenerationDialogViewAndroid(
    PasswordGenerationController* controller)
     : controller_(controller) {
  ui::WindowAndroid* window_android = controller_->top_level_native_window();
 
   DCHECK(window_android);
   java_object_.Reset(Java_PasswordGenerationDialogBridge_create(
      base::android::AttachCurrentThread(), window_android->GetJavaObject(),
      reinterpret_cast<long>(this)));
}
