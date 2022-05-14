bool DebuggerAttachFunction::RunAsync() {
  scoped_ptr<Attach::Params> params(Attach::Params::Create(*args_));
  EXTENSION_FUNCTION_VALIDATE(params.get());

  CopyDebuggee(&debuggee_, params->target);
  if (!InitAgentHost())
    return false;

  if (!DevToolsHttpHandler::IsSupportedProtocolVersion(
          params->required_version)) {
    error_ = ErrorUtils::FormatErrorMessage(
        keys::kProtocolVersionNotSupportedError,
        params->required_version);
    return false;
  }

  if (agent_host_->IsAttached()) {
    FormatErrorMessage(keys::kAlreadyAttachedError);
     return false;
   }
 
   infobars::InfoBar* infobar = NULL;
   if (!CommandLine::ForCurrentProcess()->
       HasSwitch(switches::kSilentDebuggerExtensionAPI)) {
     infobar = ExtensionDevToolsInfoBarDelegate::Create(
        agent_host_->GetRenderViewHost(), GetExtension()->name());
     if (!infobar) {
       error_ = ErrorUtils::FormatErrorMessage(
           keys::kSilentDebuggingRequired,
          switches::kSilentDebuggerExtensionAPI);
       return false;
     }
   }
 
   new ExtensionDevToolsClientHost(GetProfile(),
                                   agent_host_.get(),
                                  GetExtension()->id(),
                                  GetExtension()->name(),
                                   debuggee_,
                                   infobar);
   SendResponse(true);
  return true;
}
