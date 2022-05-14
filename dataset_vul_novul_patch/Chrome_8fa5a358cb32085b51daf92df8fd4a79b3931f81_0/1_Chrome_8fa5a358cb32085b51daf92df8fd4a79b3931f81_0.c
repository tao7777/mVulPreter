 bool PrintWebViewHelper::OnMessageReceived(const IPC::Message& message) {
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
   return handled;
 }
