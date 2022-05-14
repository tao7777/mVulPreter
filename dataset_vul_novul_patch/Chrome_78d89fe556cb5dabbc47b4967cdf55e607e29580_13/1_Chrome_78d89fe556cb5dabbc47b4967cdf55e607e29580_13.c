 UnacceleratedStaticBitmapImage::MakeAccelerated(
     base::WeakPtr<WebGraphicsContext3DProviderWrapper> context_wrapper) {
   if (!context_wrapper)
     return nullptr;  // Can happen if the context is lost.
 
  GrContext* grcontext = context_wrapper->ContextProvider()->GetGrContext();
  if (!grcontext)
    return nullptr;  // Can happen if the context is lost.

  sk_sp<SkImage> sk_image = paint_image_.GetSkImage();
  sk_sp<SkImage> gpu_skimage =
      sk_image->makeTextureImage(grcontext, sk_image->colorSpace());
  if (!gpu_skimage)
    return nullptr;

  return AcceleratedStaticBitmapImage::CreateFromSkImage(
      std::move(gpu_skimage), std::move(context_wrapper));
}
