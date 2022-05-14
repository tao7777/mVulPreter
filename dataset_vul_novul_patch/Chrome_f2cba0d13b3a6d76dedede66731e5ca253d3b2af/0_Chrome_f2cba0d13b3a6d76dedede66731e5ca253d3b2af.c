ConnectionInfoPopupAndroid::ConnectionInfoPopupAndroid(
    JNIEnv* env,
    jobject java_website_settings_pop,
    WebContents* web_contents) {
  content::NavigationEntry* nav_entry =
      web_contents->GetController().GetVisibleEntry();
  if (nav_entry == NULL)
    return;

  popup_jobject_.Reset(env, java_website_settings_pop);

  presenter_.reset(new WebsiteSettings(
       this,
       Profile::FromBrowserContext(web_contents->GetBrowserContext()),
       TabSpecificContentSettings::FromWebContents(web_contents),
      web_contents,
       nav_entry->GetURL(),
       nav_entry->GetSSL(),
       content::CertStore::GetInstance()));
}
