 bool DebuggerFunction::InitAgentHost() {
  const Extension* extension = GetExtension();
   if (debuggee_.tab_id) {
     WebContents* web_contents = NULL;
     bool result = ExtensionTabUtil::GetTabById(*debuggee_.tab_id,
                                               GetProfile(),
                                               include_incognito(),
                                               NULL,
                                               NULL,
                                                &web_contents,
                                                NULL);
     if (result && web_contents) {
      // TODO(rdevlin.cronin) This should definitely be GetLastCommittedURL().
      GURL url = web_contents->GetVisibleURL();
      if (PermissionsData::IsRestrictedUrl(url, url, extension, &error_))
         return false;
       agent_host_ = DevToolsAgentHost::GetOrCreateFor(web_contents);
     }
   } else if (debuggee_.extension_id) {
    ExtensionHost* extension_host =
        ExtensionSystem::Get(GetProfile())
             ->process_manager()
             ->GetBackgroundHostForExtension(*debuggee_.extension_id);
     if (extension_host) {
      if (PermissionsData::IsRestrictedUrl(extension_host->GetURL(),
                                           extension_host->GetURL(),
                                           extension,
                                           &error_)) {
        return false;
      }
       agent_host_ = DevToolsAgentHost::GetOrCreateFor(
           extension_host->render_view_host());
     }
  } else if (debuggee_.target_id) {
    agent_host_ = DevToolsAgentHost::GetForId(*debuggee_.target_id);
  } else {
    error_ = keys::kInvalidTargetError;
    return false;
  }

  if (!agent_host_.get()) {
    FormatErrorMessage(keys::kNoTargetError);
    return false;
  }
  return true;
}
