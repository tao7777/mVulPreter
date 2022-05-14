 void ChromeMockRenderThread::OnGetDefaultPrintSettings(
     PrintMsg_Print_Params* params) {
  printer_->GetDefaultPrintSettings(params);
 }
