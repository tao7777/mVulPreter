 void ChromeContentBrowserClient::OpenURL(
    content::SiteInstance* site_instance,
     const content::OpenURLParams& params,
    const base::RepeatingCallback<void(content::WebContents*)>& callback) {
   DCHECK_CURRENTLY_ON(BrowserThread::UI);
  DCHECK(ShouldAllowOpenURL(site_instance, params.url));

  content::BrowserContext* browser_context = site_instance->GetBrowserContext();
 
 #if defined(OS_ANDROID)
   ServiceTabLauncher::GetInstance()->LaunchTab(browser_context, params,
                                               callback);
#else
  NavigateParams nav_params(Profile::FromBrowserContext(browser_context),
                            params.url, params.transition);
  nav_params.FillNavigateParamsFromOpenURLParams(params);
  nav_params.user_gesture = params.user_gesture;

  Navigate(&nav_params);
  callback.Run(nav_params.navigated_or_inserted_contents);
#endif
}
