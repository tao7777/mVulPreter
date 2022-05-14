void ExtensionWebContentsObserver::RenderViewCreated(
    content::RenderViewHost* render_view_host) {
  const Extension* extension = GetExtension(render_view_host);
   if (!extension)
     return;
 
  content::RenderProcessHost* process = render_view_host->GetProcess();
  if (type == Manifest::TYPE_EXTENSION ||
      type == Manifest::TYPE_LEGACY_PACKAGED_APP ||
      (type == Manifest::TYPE_PLATFORM_APP &&
       extension->location() == Manifest::COMPONENT)) {
    content::ChildProcessSecurityPolicy::GetInstance()->GrantScheme(
        process->GetID(), content::kChromeUIScheme);
  }
 
   if (type == Manifest::TYPE_EXTENSION ||
       type == Manifest::TYPE_LEGACY_PACKAGED_APP) {
     ExtensionPrefs* prefs = ExtensionPrefs::Get(browser_context_);
     if (prefs->AllowFileAccess(extension->id())) {
       content::ChildProcessSecurityPolicy::GetInstance()->GrantScheme(
          process->GetID(), url::kFileScheme);
     }
   }
 
  render_view_host->Send(new ExtensionMsg_ActivateExtension(extension->id()));
}
