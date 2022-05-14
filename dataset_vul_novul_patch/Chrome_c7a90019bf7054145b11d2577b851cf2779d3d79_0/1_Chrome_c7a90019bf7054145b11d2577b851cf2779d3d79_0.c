void PrintingMessageFilter::OnUpdatePrintSettings(
     int document_cookie, const DictionaryValue& job_settings,
     IPC::Message* reply_msg) {
   scoped_refptr<printing::PrinterQuery> printer_query;
  print_job_manager_->PopPrinterQuery(document_cookie, &printer_query);
  if (printer_query.get()) {
    CancelableTask* task = NewRunnableMethod(
        this,
        &PrintingMessageFilter::OnUpdatePrintSettingsReply,
        printer_query,
        reply_msg);
    printer_query->SetSettings(job_settings, task);
   }
 }
