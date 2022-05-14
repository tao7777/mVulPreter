 void AcceleratedStaticBitmapImage::Transfer() {
  CheckThread();
   EnsureMailbox(kVerifiedSyncToken, GL_NEAREST);
  detach_thread_at_next_check_ = true;
 }
