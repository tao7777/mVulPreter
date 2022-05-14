 bool Plugin::LoadNaClModuleContinuationIntern(ErrorInfo* error_info) {
  if (using_ipc_proxy_)
    return true;
   if (!main_subprocess_.StartSrpcServices()) {
     error_info->SetReport(ERROR_SRPC_CONNECTION_FAIL,
                           "SRPC connection failure for " +
                          main_subprocess_.description());
    return false;
  }
  if (!main_subprocess_.StartJSObjectProxy(this, error_info)) {
    return false;
  }
  PLUGIN_PRINTF(("Plugin::LoadNaClModule (%s)\n",
                 main_subprocess_.detailed_description().c_str()));
  return true;
}
