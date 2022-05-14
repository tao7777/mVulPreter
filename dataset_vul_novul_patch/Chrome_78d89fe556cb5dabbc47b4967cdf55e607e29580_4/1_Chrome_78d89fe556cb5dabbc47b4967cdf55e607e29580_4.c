 void AcceleratedStaticBitmapImage::CreateImageFromMailboxIfNeeded() {
   if (texture_holder_->IsSkiaTextureHolder())
     return;
   texture_holder_ =
      std::make_unique<SkiaTextureHolder>(std::move(texture_holder_));
}
