void DestroySkImageOnOriginalThread(
    sk_sp<SkImage> image,
    base::WeakPtr<WebGraphicsContext3DProviderWrapper> context_provider_wrapper,
    std::unique_ptr<gpu::SyncToken> sync_token) {
  if (context_provider_wrapper &&
      image->isValid(
          context_provider_wrapper->ContextProvider()->GetGrContext())) {
    if (sync_token->HasData()) {
      context_provider_wrapper->ContextProvider()
          ->ContextGL()
          ->WaitSyncTokenCHROMIUM(sync_token->GetData());
    }
     image->getTexture()->textureParamsModified();
   }
 }
