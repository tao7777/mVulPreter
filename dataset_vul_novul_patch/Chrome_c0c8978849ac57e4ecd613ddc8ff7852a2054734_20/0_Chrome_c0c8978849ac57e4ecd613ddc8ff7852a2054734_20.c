 bool PlatformSensorProviderBase::CreateSharedBufferIfNeeded() {
   DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
  if (shared_buffer_mapping_.get())
     return true;
 
  if (!shared_buffer_handle_.is_valid()) {
    shared_buffer_handle_ =
        mojo::SharedBufferHandle::Create(kSharedBufferSizeInBytes);
    if (!shared_buffer_handle_.is_valid())
      return false;
  }

  // Create a writable mapping for the buffer as soon as possible, that will be
  // used by all platform sensor implementations that want to update it. Note
  // that on Android, cloning the shared memory handle readonly (as performed
  // by CloneSharedBufferHandle()) will seal the region read-only, preventing
  // future writable mappings to be created (but this one will survive).
  shared_buffer_mapping_ = shared_buffer_handle_->Map(kSharedBufferSizeInBytes);
  return shared_buffer_mapping_.get() != nullptr;
 }
