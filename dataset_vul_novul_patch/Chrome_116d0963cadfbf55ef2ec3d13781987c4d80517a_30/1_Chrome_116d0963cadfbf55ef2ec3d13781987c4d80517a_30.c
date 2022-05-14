 void ChromeMockRenderThread::OnDidPrintPage(
     const PrintHostMsg_DidPrintPage_Params& params) {
  if (printer_.get())
    printer_->PrintPage(params);
 }
