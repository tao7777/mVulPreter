ExtensionDevToolsClientHost::ExtensionDevToolsClientHost(
    Profile* profile,
    DevToolsAgentHost* agent_host,
    const std::string& extension_id,
    const std::string& extension_name,
    const Debuggee& debuggee)
     : profile_(profile),
       agent_host_(agent_host),
       extension_id_(extension_id),
      extension_name_(extension_name),
       last_request_id_(0),
       infobar_(nullptr),
       detach_reason_(api::debugger::DETACH_REASON_TARGET_CLOSED),
      extension_registry_observer_(this) {
  CopyDebuggee(&debuggee_, debuggee);

  g_attached_client_hosts.Get().insert(this);

  extension_registry_observer_.Add(ExtensionRegistry::Get(profile_));

   registrar_.Add(this, chrome::NOTIFICATION_APP_TERMINATING,
                  content::NotificationService::AllSources());
}
 
bool ExtensionDevToolsClientHost::Attach() {
  if (!agent_host_->AttachRestrictedClient(this))
    return false;
 
   if (base::CommandLine::ForCurrentProcess()->HasSwitch(
           ::switches::kSilentDebuggerExtensionAPI)) {
    return true;
   }
 
   const Extension* extension =
      ExtensionRegistry::Get(profile_)->enabled_extensions().GetByID(
          extension_id_);
  // TODO(dgozman): null-checking |extension| below is sketchy.
  // We probably should not allow debugging in this case. Or maybe
  // it's never null?
   if (extension && Manifest::IsPolicyLocation(extension->location()))
    return true;
 
   infobar_ = ExtensionDevToolsInfoBar::Create(
      extension_id_, extension_name_, this,
       base::Bind(&ExtensionDevToolsClientHost::InfoBarDismissed,
                  base::Unretained(this)));
  return true;
 }
