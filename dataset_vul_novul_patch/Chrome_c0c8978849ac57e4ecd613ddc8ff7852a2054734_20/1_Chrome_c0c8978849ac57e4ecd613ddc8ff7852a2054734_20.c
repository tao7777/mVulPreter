 bool PlatformSensorProviderBase::CreateSharedBufferIfNeeded() {
   DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
  if (shared_buffer_handle_.is_valid())
     return true;
 
  shared_buffer_handle_ =
      mojo::SharedBufferHandle::Create(kSharedBufferSizeInBytes);
  return shared_buffer_handle_.is_valid();
 }
