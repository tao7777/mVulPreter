 VideoCaptureManager::~VideoCaptureManager() {
  DCHECK(controllers_.empty());
   DCHECK(device_start_request_queue_.empty());
   if (screenlock_monitor_) {
     screenlock_monitor_->RemoveObserver(this);
  }
}
