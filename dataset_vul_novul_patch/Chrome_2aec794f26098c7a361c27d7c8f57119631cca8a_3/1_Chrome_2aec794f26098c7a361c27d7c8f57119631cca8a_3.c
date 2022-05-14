bool DebuggerDetachFunction::RunAsync() {
  std::unique_ptr<Detach::Params> params(Detach::Params::Create(*args_));
  EXTENSION_FUNCTION_VALIDATE(params.get());

  CopyDebuggee(&debuggee_, params->target);
   if (!InitClientHost())
     return false;
 
   client_host_->Close();
   SendResponse(true);
   return true;
}
