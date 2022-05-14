 void ChromeMockRenderThread::OnDidGetPrintedPagesCount(
     int cookie, int number_pages) {
  if (printer_.get())
    printer_->SetPrintedPagesCount(cookie, number_pages);
 }
