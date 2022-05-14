CommandBufferProxyImpl::AllocateAndMapSharedMemory(size_t size) {
  mojo::ScopedSharedBufferHandle handle =
      mojo::SharedBufferHandle::Create(size);
  if (!handle.is_valid()) {
    DLOG(ERROR) << "AllocateAndMapSharedMemory: Create failed";
    return nullptr;
  }
 
   base::SharedMemoryHandle platform_handle;
   size_t shared_memory_size;
  bool readonly;
   MojoResult result = mojo::UnwrapSharedMemoryHandle(
      std::move(handle), &platform_handle, &shared_memory_size, &readonly);
   if (result != MOJO_RESULT_OK) {
     DLOG(ERROR) << "AllocateAndMapSharedMemory: Unwrap failed";
     return nullptr;
   }
   DCHECK_EQ(shared_memory_size, size);
 
  auto shm = std::make_unique<base::SharedMemory>(platform_handle, readonly);
   if (!shm->Map(size)) {
     DLOG(ERROR) << "AllocateAndMapSharedMemory: Map failed";
     return nullptr;
  }

  return shm;
}
