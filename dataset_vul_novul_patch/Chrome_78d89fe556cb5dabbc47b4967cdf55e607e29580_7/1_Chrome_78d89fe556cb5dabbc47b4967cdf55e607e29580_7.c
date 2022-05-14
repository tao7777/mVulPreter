 void AcceleratedStaticBitmapImage::EnsureMailbox(MailboxSyncMode mode,
                                                  GLenum filter) {
   if (!texture_holder_->IsMailboxTextureHolder()) {
     TRACE_EVENT0("blink", "AcceleratedStaticBitmapImage::EnsureMailbox");
 
    if (!original_skia_image_) {
      RetainOriginalSkImage();
    }

    texture_holder_ = std::make_unique<MailboxTextureHolder>(
        std::move(texture_holder_), filter);
  }
  texture_holder_->Sync(mode);
 }
