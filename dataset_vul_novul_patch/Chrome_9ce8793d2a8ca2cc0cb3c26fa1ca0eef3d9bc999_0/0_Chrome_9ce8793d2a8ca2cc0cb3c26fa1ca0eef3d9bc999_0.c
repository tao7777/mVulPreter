void PPB_URLLoader_Impl::RunCallback(int32_t result) {
  if (!pending_callback_.get()) {
     CHECK(main_document_loader_);
     return;
   }

  // If |user_buffer_| was set as part of registering the callback, ensure
  // it got cleared since the callback is now free to delete it.
  DCHECK(!user_buffer_);
   TrackedCallback::ClearAndRun(&pending_callback_, result);
 }
