 void ChromeMockRenderThread::OnGetDefaultPrintSettings(
     PrintMsg_Print_Params* params) {
  if (printer_.get())
    printer_->GetDefaultPrintSettings(params);
 }
