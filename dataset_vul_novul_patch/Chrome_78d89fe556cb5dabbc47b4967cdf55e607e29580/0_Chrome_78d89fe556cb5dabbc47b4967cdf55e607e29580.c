 void AcceleratedStaticBitmapImage::Abandon() {
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
   texture_holder_->Abandon();
 }
