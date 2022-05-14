ChromeContentBrowserClient::CreateThrottlesForNavigation(
    content::NavigationHandle* handle) {
  std::vector<std::unique_ptr<content::NavigationThrottle>> throttles;

  if (handle->IsInMainFrame()) {
    throttles.push_back(
        page_load_metrics::MetricsNavigationThrottle::Create(handle));
  }

#if BUILDFLAG(ENABLE_PLUGINS)
  std::unique_ptr<content::NavigationThrottle> flash_url_throttle =
      FlashDownloadInterception::MaybeCreateThrottleFor(handle);
  if (flash_url_throttle)
    throttles.push_back(std::move(flash_url_throttle));
#endif

#if BUILDFLAG(ENABLE_SUPERVISED_USERS)
  std::unique_ptr<content::NavigationThrottle> supervised_user_throttle =
      SupervisedUserNavigationThrottle::MaybeCreateThrottleFor(handle);
  if (supervised_user_throttle)
    throttles.push_back(std::move(supervised_user_throttle));
#endif

#if defined(OS_ANDROID)
  prerender::PrerenderContents* prerender_contents =
      prerender::PrerenderContents::FromWebContents(handle->GetWebContents());
  if (!prerender_contents && handle->IsInMainFrame()) {
    throttles.push_back(
        navigation_interception::InterceptNavigationDelegate::CreateThrottleFor(
            handle));
  }
  throttles.push_back(InterceptOMADownloadNavigationThrottle::Create(handle));
#elif BUILDFLAG(ENABLE_EXTENSIONS)
  if (handle->IsInMainFrame()) {
    auto url_to_app_throttle =
        PlatformAppNavigationRedirector::MaybeCreateThrottleFor(handle);
    if (url_to_app_throttle)
      throttles.push_back(std::move(url_to_app_throttle));
  }

  if (base::FeatureList::IsEnabled(features::kDesktopPWAWindowing)) {
    if (base::FeatureList::IsEnabled(features::kDesktopPWAsLinkCapturing)) {
      auto bookmark_app_experimental_throttle =
          extensions::BookmarkAppExperimentalNavigationThrottle::
              MaybeCreateThrottleFor(handle);
      if (bookmark_app_experimental_throttle)
        throttles.push_back(std::move(bookmark_app_experimental_throttle));
    } else if (!base::FeatureList::IsEnabled(
                   features::kDesktopPWAsStayInWindow)) {
      auto bookmark_app_throttle =
          extensions::BookmarkAppNavigationThrottle::MaybeCreateThrottleFor(
              handle);
      if (bookmark_app_throttle)
        throttles.push_back(std::move(bookmark_app_throttle));
    }
  }
   if (base::FeatureList::IsEnabled(
           features::kMimeHandlerViewInCrossProcessFrame)) {
     auto plugin_frame_attach_throttle =
        extensions::ExtensionsGuestViewMessageFilter::MaybeCreateThrottle(
            handle);
     if (plugin_frame_attach_throttle)
       throttles.push_back(std::move(plugin_frame_attach_throttle));
   }
#endif

#if defined(OS_CHROMEOS)
  if (handle->IsInMainFrame()) {
    if (merge_session_throttling_utils::ShouldAttachNavigationThrottle() &&
        !merge_session_throttling_utils::AreAllSessionMergedAlready() &&
        handle->GetURL().SchemeIsHTTPOrHTTPS()) {
      throttles.push_back(MergeSessionNavigationThrottle::Create(handle));
    }

    auto url_to_apps_throttle =
        chromeos::AppsNavigationThrottle::MaybeCreate(handle);
    if (url_to_apps_throttle)
      throttles.push_back(std::move(url_to_apps_throttle));
  }
#endif

#if BUILDFLAG(ENABLE_EXTENSIONS)
  throttles.push_back(
      std::make_unique<extensions::ExtensionNavigationThrottle>(handle));

  std::unique_ptr<content::NavigationThrottle> user_script_throttle =
      extensions::ExtensionsBrowserClient::Get()
          ->GetUserScriptListener()
          ->CreateNavigationThrottle(handle);
  if (user_script_throttle)
    throttles.push_back(std::move(user_script_throttle));
#endif

#if BUILDFLAG(ENABLE_SUPERVISED_USERS)
  std::unique_ptr<content::NavigationThrottle> supervised_user_nav_throttle =
      SupervisedUserGoogleAuthNavigationThrottle::MaybeCreate(handle);
  if (supervised_user_nav_throttle)
    throttles.push_back(std::move(supervised_user_nav_throttle));
#endif

  content::WebContents* web_contents = handle->GetWebContents();
  if (auto* subresource_filter_client =
          ChromeSubresourceFilterClient::FromWebContents(web_contents)) {
    subresource_filter_client->MaybeAppendNavigationThrottles(handle,
                                                              &throttles);
  }

#if !defined(OS_ANDROID)
  std::unique_ptr<content::NavigationThrottle>
      background_tab_navigation_throttle = resource_coordinator::
          BackgroundTabNavigationThrottle::MaybeCreateThrottleFor(handle);
  if (background_tab_navigation_throttle)
    throttles.push_back(std::move(background_tab_navigation_throttle));
#endif

#if defined(SAFE_BROWSING_DB_LOCAL)
  std::unique_ptr<content::NavigationThrottle>
      password_protection_navigation_throttle =
          safe_browsing::MaybeCreateNavigationThrottle(handle);
  if (password_protection_navigation_throttle) {
    throttles.push_back(std::move(password_protection_navigation_throttle));
  }
#endif

  std::unique_ptr<content::NavigationThrottle> pdf_iframe_throttle =
      PDFIFrameNavigationThrottle::MaybeCreateThrottleFor(handle);
  if (pdf_iframe_throttle)
    throttles.push_back(std::move(pdf_iframe_throttle));

  std::unique_ptr<content::NavigationThrottle> tab_under_throttle =
      TabUnderNavigationThrottle::MaybeCreate(handle);
  if (tab_under_throttle)
    throttles.push_back(std::move(tab_under_throttle));

  throttles.push_back(std::make_unique<PolicyBlacklistNavigationThrottle>(
      handle, handle->GetWebContents()->GetBrowserContext()));

  if (base::FeatureList::IsEnabled(features::kSSLCommittedInterstitials)) {
    throttles.push_back(std::make_unique<SSLErrorNavigationThrottle>(
        handle,
        std::make_unique<CertificateReportingServiceCertReporter>(web_contents),
        base::Bind(&SSLErrorHandler::HandleSSLError)));
  }

  std::unique_ptr<content::NavigationThrottle> https_upgrade_timing_throttle =
      TypedNavigationTimingThrottle::MaybeCreateThrottleFor(handle);
  if (https_upgrade_timing_throttle)
    throttles.push_back(std::move(https_upgrade_timing_throttle));

#if !defined(OS_ANDROID)
  std::unique_ptr<content::NavigationThrottle> devtools_throttle =
      DevToolsWindow::MaybeCreateNavigationThrottle(handle);
  if (devtools_throttle)
    throttles.push_back(std::move(devtools_throttle));

  std::unique_ptr<content::NavigationThrottle> new_tab_page_throttle =
      NewTabPageNavigationThrottle::MaybeCreateThrottleFor(handle);
  if (new_tab_page_throttle)
    throttles.push_back(std::move(new_tab_page_throttle));

  std::unique_ptr<content::NavigationThrottle>
      google_password_manager_throttle =
          GooglePasswordManagerNavigationThrottle::MaybeCreateThrottleFor(
              handle);
  if (google_password_manager_throttle)
    throttles.push_back(std::move(google_password_manager_throttle));
#endif

  std::unique_ptr<content::NavigationThrottle> previews_lite_page_throttle =
      PreviewsLitePageDecider::MaybeCreateThrottleFor(handle);
  if (previews_lite_page_throttle)
    throttles.push_back(std::move(previews_lite_page_throttle));
  if (base::FeatureList::IsEnabled(safe_browsing::kCommittedSBInterstitials)) {
    throttles.push_back(
        std::make_unique<safe_browsing::SafeBrowsingNavigationThrottle>(
            handle));
  }

#if defined(OS_WIN) || defined(OS_MACOSX) || \
    (defined(OS_LINUX) && !defined(OS_CHROMEOS))
  std::unique_ptr<content::NavigationThrottle> browser_switcher_throttle =
      browser_switcher::BrowserSwitcherNavigationThrottle ::
          MaybeCreateThrottleFor(handle);
  if (browser_switcher_throttle)
    throttles.push_back(std::move(browser_switcher_throttle));
#endif

  return throttles;
}
