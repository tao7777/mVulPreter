 void ResourceMessageFilter::OnScriptedPrint(
     const ViewHostMsg_ScriptedPrint_Params& params,
     IPC::Message* reply_msg) {
#if defined(OS_WIN)
  HWND host_window = gfx::NativeViewFromId(params.host_window_id);
#elif defined(OS_MACOSX)
  gfx::NativeWindow host_window = NULL;
#endif
 
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
#if defined(OS_WIN)
  if (!host_window || !IsWindow(host_window)) {
    host_window = GetDesktopWindow();
  } else {
    host_window = GetAncestor(host_window, GA_ROOTOWNER);
  }
  DCHECK(host_window);
#endif
 
   printer_query->GetSettings(printing::PrinterQuery::ASK_USER,
                             host_window,
                              params.expected_pages_count,
                              params.has_selection,
                              params.use_overlays,
                             task);
}
