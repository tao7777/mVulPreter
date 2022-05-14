void PrintingMessageFilter::OnUpdatePrintSettings(
     int document_cookie, const DictionaryValue& job_settings,
     IPC::Message* reply_msg) {
   scoped_refptr<printing::PrinterQuery> printer_query;
  if (!print_job_manager_->printing_enabled()) {
    // Reply with NULL query.
    OnUpdatePrintSettingsReply(printer_query, reply_msg);
    return;
   }

  print_job_manager_->PopPrinterQuery(document_cookie, &printer_query);
  if (!printer_query.get())
    printer_query = new printing::PrinterQuery;
  CancelableTask* task = NewRunnableMethod(
      this,
      &PrintingMessageFilter::OnUpdatePrintSettingsReply,
      printer_query,
      reply_msg);
  printer_query->SetSettings(job_settings, task);
 }
