 void PlatformSensorProviderBase::FreeResourcesIfNeeded() {
   DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
   if (sensor_map_.empty() && requests_map_.empty()) {
     FreeResources();
     shared_buffer_handle_.reset();
   }
 }
