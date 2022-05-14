 void AcceleratedStaticBitmapImage::Transfer() {
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
   EnsureMailbox(kVerifiedSyncToken, GL_NEAREST);
  DETACH_FROM_THREAD(thread_checker_);
 }
