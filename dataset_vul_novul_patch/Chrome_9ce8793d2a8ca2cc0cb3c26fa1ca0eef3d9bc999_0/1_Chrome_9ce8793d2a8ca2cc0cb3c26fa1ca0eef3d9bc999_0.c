void PPB_URLLoader_Impl::RunCallback(int32_t result) {
  if (!pending_callback_.get()) {
     CHECK(main_document_loader_);
     return;
   }
   TrackedCallback::ClearAndRun(&pending_callback_, result);
 }
