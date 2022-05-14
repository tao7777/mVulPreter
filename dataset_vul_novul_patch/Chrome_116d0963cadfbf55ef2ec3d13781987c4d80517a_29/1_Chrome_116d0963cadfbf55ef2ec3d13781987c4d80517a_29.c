 void ChromeMockRenderThread::OnDidPreviewPage(
     const PrintHostMsg_DidPreviewPage_Params& params) {
  DCHECK(params.page_number >= printing::FIRST_PAGE_INDEX);
   print_preview_pages_remaining_--;
 }
