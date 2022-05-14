AcceleratedStaticBitmapImage::AcceleratedStaticBitmapImage(
    sk_sp<SkImage> image,
    base::WeakPtr<WebGraphicsContext3DProviderWrapper>&&
        context_provider_wrapper)
    : paint_image_content_id_(cc::PaintImage::GetNextContentId()) {
   CHECK(image && image->isTextureBacked());
   texture_holder_ = std::make_unique<SkiaTextureHolder>(
       std::move(image), std::move(context_provider_wrapper));
  thread_checker_.DetachFromThread();
 }
