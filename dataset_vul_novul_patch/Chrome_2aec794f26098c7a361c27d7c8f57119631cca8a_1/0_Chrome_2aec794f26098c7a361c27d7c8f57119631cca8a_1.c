 void ExtensionDevToolsClientHost::InfoBarDismissed() {
   detach_reason_ = api::debugger::DETACH_REASON_CANCELED_BY_USER;
  RespondDetachedToPendingRequests();
   SendDetachedEvent();
   Close();
 }
