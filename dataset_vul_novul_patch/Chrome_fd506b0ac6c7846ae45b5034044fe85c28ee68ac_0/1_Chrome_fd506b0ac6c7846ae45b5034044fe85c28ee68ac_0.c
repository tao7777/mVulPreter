void FrameLoader::StopAllLoaders() {
  if (frame_->GetDocument()->PageDismissalEventBeingDispatched() !=
      Document::kNoDismissal)
    return;

   if (in_stop_all_loaders_)
     return;
 
  in_stop_all_loaders_ = true;
 
   for (Frame* child = frame_->Tree().FirstChild(); child;
        child = child->Tree().NextSibling()) {
    if (child->IsLocalFrame())
      ToLocalFrame(child)->Loader().StopAllLoaders();
  }
 
   frame_->GetDocument()->CancelParsing();
   if (document_loader_)
    document_loader_->Fetcher()->StopFetching();
   if (!protect_provisional_loader_)
     DetachDocumentLoader(provisional_document_loader_);
   frame_->GetNavigationScheduler().Cancel();
  if (document_loader_ && !document_loader_->SentDidFinishLoad()) {
    document_loader_->LoadFailed(
        ResourceError::CancelledError(document_loader_->Url()));
  }
  in_stop_all_loaders_ = false;
 
   TakeObjectSnapshot();
 }
