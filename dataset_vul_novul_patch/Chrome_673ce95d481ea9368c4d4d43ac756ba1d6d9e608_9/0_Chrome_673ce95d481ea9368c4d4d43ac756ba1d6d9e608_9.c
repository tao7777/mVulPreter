uint32_t ClientSharedBitmapManager::NotifyAllocatedSharedBitmap(
    base::SharedMemory* memory,
    const SharedBitmapId& id) {
  base::SharedMemoryHandle handle_to_send =
      base::SharedMemory::DuplicateHandle(memory->handle());
  if (!base::SharedMemory::IsHandleValid(handle_to_send)) {
    LOG(ERROR) << "Failed to duplicate shared memory handle for bitmap.";
    return 0;
   }
 
   mojo::ScopedSharedBufferHandle buffer_handle = mojo::WrapSharedMemoryHandle(
      handle_to_send, memory->mapped_size(),
      mojo::UnwrappedSharedMemoryHandleProtection::kReadWrite);
 
   {
     base::AutoLock lock(lock_);
    (*shared_bitmap_allocation_notifier_)
        ->DidAllocateSharedBitmap(std::move(buffer_handle), id);
    return ++last_sequence_number_;
  }
}
