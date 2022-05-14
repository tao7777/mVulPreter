void ExtensionWebContentsObserver::RenderViewCreated(
    content::RenderViewHost* render_view_host) {
  const Extension* extension = GetExtension(render_view_host);
   if (!extension)
     return;
 
 
   if (type == Manifest::TYPE_EXTENSION ||
       type == Manifest::TYPE_LEGACY_PACKAGED_APP) {
     ExtensionPrefs* prefs = ExtensionPrefs::Get(browser_context_);
     if (prefs->AllowFileAccess(extension->id())) {
       content::ChildProcessSecurityPolicy::GetInstance()->GrantScheme(
          render_view_host->GetProcess()->GetID(), url::kFileScheme);
     }
   }
 
  render_view_host->Send(new ExtensionMsg_ActivateExtension(extension->id()));
}
