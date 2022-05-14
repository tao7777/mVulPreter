bool DebuggerAttachFunction::RunAsync() {
  std::unique_ptr<Attach::Params> params(Attach::Params::Create(*args_));
  EXTENSION_FUNCTION_VALIDATE(params.get());

  CopyDebuggee(&debuggee_, params->target);
  if (!InitAgentHost())
    return false;

  if (!DevToolsAgentHost::IsSupportedProtocolVersion(
          params->required_version)) {
    error_ = ErrorUtils::FormatErrorMessage(
        keys::kProtocolVersionNotSupportedError,
        params->required_version);
    return false;
  }

  if (FindClientHost()) {
    FormatErrorMessage(keys::kAlreadyAttachedError);
     return false;
   }
 
  new ExtensionDevToolsClientHost(GetProfile(), agent_host_.get(),
                                  extension()->id(), extension()->name(),
                                  debuggee_);
   SendResponse(true);
   return true;
 }
