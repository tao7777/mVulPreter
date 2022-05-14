void PrintingMessageFilter::OnCheckForCancel(const std::string& preview_ui_addr,
                                              int preview_request_id,
                                              bool* cancel) {
  PrintPreviewUI::GetCurrentPrintPreviewStatus(preview_ui_addr,
                                                preview_request_id,
                                                cancel);
 }
