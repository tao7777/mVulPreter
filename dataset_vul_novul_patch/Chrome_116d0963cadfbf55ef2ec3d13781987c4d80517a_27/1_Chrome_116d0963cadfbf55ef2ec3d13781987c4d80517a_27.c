void ChromeMockRenderThread::OnCheckForCancel(
    const std::string& preview_ui_addr,
    int preview_request_id,
    bool* cancel) {
   *cancel =
       (print_preview_pages_remaining_ == print_preview_cancel_page_number_);
 }
