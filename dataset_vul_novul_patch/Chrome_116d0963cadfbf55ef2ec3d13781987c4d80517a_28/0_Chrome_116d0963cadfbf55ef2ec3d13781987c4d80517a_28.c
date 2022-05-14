 void ChromeMockRenderThread::OnDidGetPrintedPagesCount(
     int cookie, int number_pages) {
  printer_->SetPrintedPagesCount(cookie, number_pages);
 }
