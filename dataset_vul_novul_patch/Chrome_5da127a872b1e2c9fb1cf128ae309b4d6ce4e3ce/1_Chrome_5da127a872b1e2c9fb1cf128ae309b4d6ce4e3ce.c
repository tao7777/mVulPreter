static void JNI_WebApkUpdateManager_StoreWebApkUpdateRequestToFile(
    JNIEnv* env,
    const JavaParamRef<jstring>& java_update_request_path,
    const JavaParamRef<jstring>& java_start_url,
    const JavaParamRef<jstring>& java_scope,
    const JavaParamRef<jstring>& java_name,
    const JavaParamRef<jstring>& java_short_name,
    const JavaParamRef<jstring>& java_primary_icon_url,
    const JavaParamRef<jobject>& java_primary_icon_bitmap,
    const JavaParamRef<jstring>& java_badge_icon_url,
    const JavaParamRef<jobject>& java_badge_icon_bitmap,
    const JavaParamRef<jobjectArray>& java_icon_urls,
    const JavaParamRef<jobjectArray>& java_icon_hashes,
    jint java_display_mode,
     jint java_orientation,
     jlong java_theme_color,
     jlong java_background_color,
     const JavaParamRef<jstring>& java_web_manifest_url,
     const JavaParamRef<jstring>& java_webapk_package,
     jint java_webapk_version,
    jboolean java_is_manifest_stale,
    jint java_update_reason,
    const JavaParamRef<jobject>& java_callback) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);

  std::string update_request_path =
      ConvertJavaStringToUTF8(env, java_update_request_path);

  ShortcutInfo info(GURL(ConvertJavaStringToUTF8(env, java_start_url)));
  info.scope = GURL(ConvertJavaStringToUTF8(env, java_scope));
  info.name = ConvertJavaStringToUTF16(env, java_name);
  info.short_name = ConvertJavaStringToUTF16(env, java_short_name);
  info.user_title = info.short_name;
  info.display = static_cast<blink::WebDisplayMode>(java_display_mode);
  info.orientation =
      static_cast<blink::WebScreenOrientationLockType>(java_orientation);
  info.theme_color = (int64_t)java_theme_color;
  info.background_color = (int64_t)java_background_color;
  info.best_primary_icon_url =
      GURL(ConvertJavaStringToUTF8(env, java_primary_icon_url));
  info.best_badge_icon_url =
       GURL(ConvertJavaStringToUTF8(env, java_badge_icon_url));
   info.manifest_url = GURL(ConvertJavaStringToUTF8(env, java_web_manifest_url));
 
   base::android::AppendJavaStringArrayToStringVector(env, java_icon_urls,
                                                      &info.icon_urls);
 
  std::vector<std::string> icon_hashes;
  base::android::AppendJavaStringArrayToStringVector(env, java_icon_hashes,
                                                     &icon_hashes);

  std::map<std::string, std::string> icon_url_to_murmur2_hash;
  for (size_t i = 0; i < info.icon_urls.size(); ++i)
    icon_url_to_murmur2_hash[info.icon_urls[i]] = icon_hashes[i];

  gfx::JavaBitmap java_primary_icon_bitmap_lock(java_primary_icon_bitmap);
  SkBitmap primary_icon =
      gfx::CreateSkBitmapFromJavaBitmap(java_primary_icon_bitmap_lock);
  primary_icon.setImmutable();

  SkBitmap badge_icon;
  if (!java_badge_icon_bitmap.is_null()) {
    gfx::JavaBitmap java_badge_icon_bitmap_lock(java_badge_icon_bitmap);
    gfx::CreateSkBitmapFromJavaBitmap(java_badge_icon_bitmap_lock);
    badge_icon.setImmutable();
  }

  std::string webapk_package;
  ConvertJavaStringToUTF8(env, java_webapk_package, &webapk_package);

  WebApkUpdateReason update_reason =
      static_cast<WebApkUpdateReason>(java_update_reason);

  WebApkInstaller::StoreUpdateRequestToFile(
      base::FilePath(update_request_path), info, primary_icon, badge_icon,
      webapk_package, std::to_string(java_webapk_version),
      icon_url_to_murmur2_hash, java_is_manifest_stale, update_reason,
      base::BindOnce(&base::android::RunBooleanCallbackAndroid,
                     ScopedJavaGlobalRef<jobject>(java_callback)));
}
