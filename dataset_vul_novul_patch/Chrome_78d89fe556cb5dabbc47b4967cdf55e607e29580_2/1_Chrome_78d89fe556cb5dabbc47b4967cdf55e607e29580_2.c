void AcceleratedStaticBitmapImage::CheckThread() {
  if (detach_thread_at_next_check_) {
    thread_checker_.DetachFromThread();
    detach_thread_at_next_check_ = false;
  }
  CHECK(thread_checker_.CalledOnValidThread());
}
