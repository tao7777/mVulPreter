 AcceleratedStaticBitmapImage::~AcceleratedStaticBitmapImage() {
   if (original_skia_image_) {
     std::unique_ptr<gpu::SyncToken> sync_token =
         base::WrapUnique(new gpu::SyncToken(texture_holder_->GetSyncToken()));
    if (original_skia_image_thread_id_ !=
        Platform::Current()->CurrentThread()->ThreadId()) {
       PostCrossThreadTask(
           *original_skia_image_task_runner_, FROM_HERE,
           CrossThreadBind(
              &DestroySkImageOnOriginalThread, std::move(original_skia_image_),
              std::move(original_skia_image_context_provider_wrapper_),
              WTF::Passed(std::move(sync_token))));
    } else {
      DestroySkImageOnOriginalThread(
          std::move(original_skia_image_),
          std::move(original_skia_image_context_provider_wrapper_),
          std::move(sync_token));
    }
  }
 }
