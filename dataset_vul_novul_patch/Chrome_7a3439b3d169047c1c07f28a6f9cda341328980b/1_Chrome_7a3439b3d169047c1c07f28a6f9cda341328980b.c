 bool PrintPreviewMessageHandler::OnMessageReceived(
     const IPC::Message& message) {
   bool handled = true;
  IPC_BEGIN_MESSAGE_MAP(PrintPreviewMessageHandler, message)
    IPC_MESSAGE_HANDLER(PrintHostMsg_RequestPrintPreview,
                        OnRequestPrintPreview)
    IPC_MESSAGE_HANDLER(PrintHostMsg_DidGetPreviewPageCount,
                        OnDidGetPreviewPageCount)
    IPC_MESSAGE_HANDLER(PrintHostMsg_DidPreviewPage,
                        OnDidPreviewPage)
    IPC_MESSAGE_HANDLER(PrintHostMsg_MetafileReadyForPrinting,
                        OnMetafileReadyForPrinting)
    IPC_MESSAGE_HANDLER(PrintHostMsg_PrintPreviewFailed,
                        OnPrintPreviewFailed)
    IPC_MESSAGE_HANDLER(PrintHostMsg_DidGetDefaultPageLayout,
                        OnDidGetDefaultPageLayout)
    IPC_MESSAGE_HANDLER(PrintHostMsg_PrintPreviewCancelled,
                         OnPrintPreviewCancelled)
     IPC_MESSAGE_HANDLER(PrintHostMsg_PrintPreviewInvalidPrinterSettings,
                         OnInvalidPrinterSettings)
     IPC_MESSAGE_UNHANDLED(handled = false)
   IPC_END_MESSAGE_MAP()
   return handled;
}
