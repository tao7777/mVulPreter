bool NaClProcessHost::OnMessageReceived(const IPC::Message& msg) {
  bool handled = true;
  IPC_BEGIN_MESSAGE_MAP(NaClProcessHost, msg)
    IPC_MESSAGE_HANDLER(NaClProcessMsg_QueryKnownToValidate,
                        OnQueryKnownToValidate)
    IPC_MESSAGE_HANDLER(NaClProcessMsg_SetKnownToValidate,
                        OnSetKnownToValidate)
#if defined(OS_WIN)
     IPC_MESSAGE_HANDLER_DELAY_REPLY(NaClProcessMsg_AttachDebugExceptionHandler,
                                     OnAttachDebugExceptionHandler)
 #endif
     IPC_MESSAGE_UNHANDLED(handled = false)
   IPC_END_MESSAGE_MAP()
   return handled;
}
