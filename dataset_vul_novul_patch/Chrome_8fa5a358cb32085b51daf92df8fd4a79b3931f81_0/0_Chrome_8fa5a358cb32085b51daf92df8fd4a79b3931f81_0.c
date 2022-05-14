 bool PrintWebViewHelper::OnMessageReceived(const IPC::Message& message) {
  // The class is not designed to handle recursive messages. This is not
  // expected during regular flow. However, during rendering of content for
  // printing, lower level code may run nested message loop. E.g. PDF may has
  // script to show message box http://crbug.com/502562. In that moment browser
  // may receive updated printer capabilities and decide to restart print
  // preview generation. When this happened message handling function may
  // choose to ignore message or safely crash process.
  ++ipc_nesting_level_;

   bool handled = true;
   IPC_BEGIN_MESSAGE_MAP(PrintWebViewHelper, message)
 #if defined(ENABLE_BASIC_PRINTING)
    IPC_MESSAGE_HANDLER(PrintMsg_PrintPages, OnPrintPages)
    IPC_MESSAGE_HANDLER(PrintMsg_PrintForSystemDialog, OnPrintForSystemDialog)
#endif  // ENABLE_BASIC_PRINTING
    IPC_MESSAGE_HANDLER(PrintMsg_InitiatePrintPreview, OnInitiatePrintPreview)
    IPC_MESSAGE_HANDLER(PrintMsg_PrintPreview, OnPrintPreview)
    IPC_MESSAGE_HANDLER(PrintMsg_PrintForPrintPreview, OnPrintForPrintPreview)
    IPC_MESSAGE_HANDLER(PrintMsg_PrintingDone, OnPrintingDone)
     IPC_MESSAGE_HANDLER(PrintMsg_SetScriptedPrintingBlocked,
                         SetScriptedPrintBlocked)
     IPC_MESSAGE_UNHANDLED(handled = false)
  IPC_END_MESSAGE_MAP()

  --ipc_nesting_level_;
   return handled;
 }
