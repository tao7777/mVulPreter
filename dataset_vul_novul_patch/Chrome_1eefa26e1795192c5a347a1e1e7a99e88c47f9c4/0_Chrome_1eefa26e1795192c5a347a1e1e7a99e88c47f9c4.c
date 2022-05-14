void ChromeExtensionWebContentsObserver::RenderViewCreated(
     content::RenderViewHost* render_view_host) {
   ReloadIfTerminated(render_view_host);
   ExtensionWebContentsObserver::RenderViewCreated(render_view_host);

  const Extension* extension = GetExtension(render_view_host);
  if (!extension)
    return;

  int process_id = render_view_host->GetProcess()->GetID();
  auto policy = content::ChildProcessSecurityPolicy::GetInstance();

  // Components of chrome that are implemented as extensions or platform apps
  // are allowed to use chrome://resources/ URLs.
  if ((extension->is_extension() || extension->is_platform_app()) &&
      Manifest::IsComponentLocation(extension->location())) {
    policy->GrantOrigin(process_id,
                        url::Origin(GURL(content::kChromeUIResourcesURL)));
  }

  // Extensions, legacy packaged apps, and component platform apps are allowed
  // to use chrome://favicon/ and chrome://extension-icon/ URLs. Hosted apps are
  // not allowed because they are served via web servers (and are generally
  // never given access to Chrome APIs).
  if (extension->is_extension() ||
      extension->is_legacy_packaged_app() ||
      (extension->is_platform_app() &&
       Manifest::IsComponentLocation(extension->location()))) {
    policy->GrantOrigin(process_id,
                        url::Origin(GURL(chrome::kChromeUIFaviconURL)));
    policy->GrantOrigin(process_id,
                        url::Origin(GURL(chrome::kChromeUIExtensionIconURL)));
  }
 }
