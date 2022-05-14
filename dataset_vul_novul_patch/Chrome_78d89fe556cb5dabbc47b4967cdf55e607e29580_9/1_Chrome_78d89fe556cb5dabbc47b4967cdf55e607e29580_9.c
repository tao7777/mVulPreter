 void AcceleratedStaticBitmapImage::RetainOriginalSkImage() {
   DCHECK(texture_holder_->IsSkiaTextureHolder());
   original_skia_image_ = texture_holder_->GetSkImage();
   original_skia_image_context_provider_wrapper_ = ContextProviderWrapper();
   DCHECK(original_skia_image_);
   Thread* thread = Platform::Current()->CurrentThread();
  original_skia_image_thread_id_ = thread->ThreadId();
   original_skia_image_task_runner_ = thread->GetTaskRunner();
 }
