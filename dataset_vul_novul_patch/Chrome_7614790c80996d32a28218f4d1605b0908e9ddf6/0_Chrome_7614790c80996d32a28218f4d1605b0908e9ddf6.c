ExtensionNavigationThrottle::WillStartOrRedirectRequest() {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  content::WebContents* web_contents = navigation_handle()->GetWebContents();
  ExtensionRegistry* registry =
      ExtensionRegistry::Get(web_contents->GetBrowserContext());

  const GURL& url = navigation_handle()->GetURL();
  bool url_has_extension_scheme = url.SchemeIs(kExtensionScheme);
  url::Origin target_origin = url::Origin::Create(url);
  const Extension* target_extension = nullptr;
  if (url_has_extension_scheme) {
    target_extension =
        registry->enabled_extensions().GetExtensionOrAppByURL(url);
  } else if (target_origin.scheme() == kExtensionScheme) {
    DCHECK(url.SchemeIsFileSystem() || url.SchemeIsBlob());
    target_extension =
        registry->enabled_extensions().GetByID(target_origin.host());
  } else {
    return content::NavigationThrottle::PROCEED;
  }

  if (!target_extension) {
    return content::NavigationThrottle::BLOCK_REQUEST;
  }

  if (target_extension->is_hosted_app()) {
    base::StringPiece resource_root_relative_path =
        url.path_piece().empty() ? base::StringPiece()
                                 : url.path_piece().substr(1);
    if (!IconsInfo::GetIcons(target_extension)
             .ContainsPath(resource_root_relative_path)) {
      return content::NavigationThrottle::BLOCK_REQUEST;
     }
   }
 
  // Block all navigations to blob: or filesystem: URLs with extension
  // origin from non-extension processes.  See https://crbug.com/645028 and
  // https://crbug.com/836858.
  bool current_frame_is_extension_process =
      !!registry->enabled_extensions().GetExtensionOrAppByURL(
          navigation_handle()->GetStartingSiteInstance()->GetSiteURL());

  if (!url_has_extension_scheme && !current_frame_is_extension_process) {
    // Relax this restriction for navigations that will result in downloads.
    // See https://crbug.com/714373.
    if (target_origin.scheme() == kExtensionScheme &&
        navigation_handle()->GetSuggestedFilename().has_value()) {
      return content::NavigationThrottle::PROCEED;
     }
 
    // Relax this restriction for apps that use <webview>.  See
    // https://crbug.com/652077.
    bool has_webview_permission =
        target_extension->permissions_data()->HasAPIPermission(
            APIPermission::kWebView);
    if (!has_webview_permission)
      return content::NavigationThrottle::CANCEL;
  }

  if (navigation_handle()->IsInMainFrame()) {
     guest_view::GuestViewBase* guest =
         guest_view::GuestViewBase::FromWebContents(web_contents);
     if (url_has_extension_scheme && guest) {
      const std::string& owner_extension_id = guest->owner_host();
      const Extension* owner_extension =
          registry->enabled_extensions().GetByID(owner_extension_id);

      std::string partition_domain;
      std::string partition_id;
      bool in_memory = false;
      bool is_guest = WebViewGuest::GetGuestPartitionConfigForSite(
          navigation_handle()->GetStartingSiteInstance()->GetSiteURL(),
          &partition_domain, &partition_id, &in_memory);

      bool allowed = true;
      url_request_util::AllowCrossRendererResourceLoadHelper(
          is_guest, target_extension, owner_extension, partition_id, url.path(),
          navigation_handle()->GetPageTransition(), &allowed);
      if (!allowed)
        return content::NavigationThrottle::BLOCK_REQUEST;
    }

    return content::NavigationThrottle::PROCEED;
  }

  content::RenderFrameHost* parent = navigation_handle()->GetParentFrame();

  bool external_ancestor = false;
  for (auto* ancestor = parent; ancestor; ancestor = ancestor->GetParent()) {
    if (ancestor->GetLastCommittedOrigin() == target_origin)
      continue;
    if (url::Origin::Create(ancestor->GetLastCommittedURL()) == target_origin)
      continue;
    if (ancestor->GetLastCommittedURL().SchemeIs(
            content::kChromeDevToolsScheme))
      continue;

    external_ancestor = true;
    break;
  }

  if (external_ancestor) {
    if (!url_has_extension_scheme)
      return content::NavigationThrottle::CANCEL;

    if (!WebAccessibleResourcesInfo::IsResourceWebAccessible(target_extension,
                                                             url.path()))
      return content::NavigationThrottle::BLOCK_REQUEST;

    if (target_extension->is_platform_app())
      return content::NavigationThrottle::CANCEL;

    const Extension* parent_extension =
        registry->enabled_extensions().GetExtensionOrAppByURL(
            parent->GetSiteInstance()->GetSiteURL());
    if (parent_extension && parent_extension->is_platform_app())
      return content::NavigationThrottle::BLOCK_REQUEST;
  }

  return content::NavigationThrottle::PROCEED;
}
