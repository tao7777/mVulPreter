void ChromeMockRenderThread::OnCheckForCancel(
void ChromeMockRenderThread::OnCheckForCancel(int32 preview_ui_id,
                                              int preview_request_id,
                                              bool* cancel) {
   *cancel =
       (print_preview_pages_remaining_ == print_preview_cancel_page_number_);
 }
