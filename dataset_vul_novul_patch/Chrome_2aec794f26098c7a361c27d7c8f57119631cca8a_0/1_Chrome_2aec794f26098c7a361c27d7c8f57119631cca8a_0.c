ExtensionDevToolsClientHost::ExtensionDevToolsClientHost(
    Profile* profile,
    DevToolsAgentHost* agent_host,
    const std::string& extension_id,
    const std::string& extension_name,
    const Debuggee& debuggee)
     : profile_(profile),
       agent_host_(agent_host),
       extension_id_(extension_id),
       last_request_id_(0),
       infobar_(nullptr),
       detach_reason_(api::debugger::DETACH_REASON_TARGET_CLOSED),
      extension_registry_observer_(this) {
  CopyDebuggee(&debuggee_, debuggee);

  g_attached_client_hosts.Get().insert(this);

  extension_registry_observer_.Add(ExtensionRegistry::Get(profile_));

   registrar_.Add(this, chrome::NOTIFICATION_APP_TERMINATING,
                  content::NotificationService::AllSources());
 
  agent_host_->AttachClient(this);
 
   if (base::CommandLine::ForCurrentProcess()->HasSwitch(
           ::switches::kSilentDebuggerExtensionAPI)) {
    return;
   }
 
   const Extension* extension =
      ExtensionRegistry::Get(profile)->enabled_extensions().GetByID(
          extension_id);
   if (extension && Manifest::IsPolicyLocation(extension->location()))
    return;
 
   infobar_ = ExtensionDevToolsInfoBar::Create(
      extension_id, extension_name, this,
       base::Bind(&ExtensionDevToolsClientHost::InfoBarDismissed,
                  base::Unretained(this)));
 }
