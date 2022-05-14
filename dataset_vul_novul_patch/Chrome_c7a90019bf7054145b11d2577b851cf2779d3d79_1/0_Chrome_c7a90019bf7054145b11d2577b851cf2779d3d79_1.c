 void PrintingMessageFilter::OnUpdatePrintSettingsReply(
     scoped_refptr<printing::PrinterQuery> printer_query,
     IPC::Message* reply_msg) {
   PrintMsg_PrintPages_Params params;
  if (!printer_query.get() ||
      printer_query->last_status() != printing::PrintingContext::OK) {
     params.Reset();
   } else {
     RenderParamsFromPrintSettings(printer_query->settings(), &params.params);
    params.params.document_cookie = printer_query->cookie();
    params.pages =
        printing::PageRange::GetPages(printer_query->settings().ranges);
  }
   PrintHostMsg_UpdatePrintSettings::WriteReplyParams(reply_msg, params);
   Send(reply_msg);
  if (printer_query.get()) {
    if (printer_query->cookie() && printer_query->settings().dpi())
      print_job_manager_->QueuePrinterQuery(printer_query.get());
    else
      printer_query->StopWorker();
  }
 }
