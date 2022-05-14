 void ChromeMockRenderThread::OnDidPrintPage(
     const PrintHostMsg_DidPrintPage_Params& params) {
  printer_->PrintPage(params);
 }
