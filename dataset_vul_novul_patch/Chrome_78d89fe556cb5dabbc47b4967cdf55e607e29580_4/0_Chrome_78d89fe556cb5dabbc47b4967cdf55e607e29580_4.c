 void AcceleratedStaticBitmapImage::CreateImageFromMailboxIfNeeded() {
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
   if (texture_holder_->IsSkiaTextureHolder())
     return;
   texture_holder_ =
      std::make_unique<SkiaTextureHolder>(std::move(texture_holder_));
}
