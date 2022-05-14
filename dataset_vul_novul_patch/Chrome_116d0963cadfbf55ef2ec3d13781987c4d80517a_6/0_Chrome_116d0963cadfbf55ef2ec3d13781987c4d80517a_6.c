void PrintingMessageFilter::OnCheckForCancel(const std::string& preview_ui_addr,
void PrintingMessageFilter::OnCheckForCancel(int32 preview_ui_id,
                                              int preview_request_id,
                                              bool* cancel) {
  PrintPreviewUI::GetCurrentPrintPreviewStatus(preview_ui_id,
                                                preview_request_id,
                                                cancel);
 }
