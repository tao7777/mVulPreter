 void ResourceMessageFilter::OnScriptedPrint(
     const ViewHostMsg_ScriptedPrint_Params& params,
     IPC::Message* reply_msg) {
  gfx::NativeView host_view =
      gfx::NativeViewFromIdInBrowser(params.host_window_id);
 
   scoped_refptr<printing::PrinterQuery> printer_query;
   print_job_manager_->PopPrinterQuery(params.cookie, &printer_query);
  if (!printer_query.get()) {
    printer_query = new printing::PrinterQuery;
  }

  CancelableTask* task = NewRunnableMethod(
      this,
      &ResourceMessageFilter::OnScriptedPrintReply,
       printer_query,
       params.routing_id,
       reply_msg);
 
   printer_query->GetSettings(printing::PrinterQuery::ASK_USER,
                             host_view,
                              params.expected_pages_count,
                              params.has_selection,
                              params.use_overlays,
                             task);
}
