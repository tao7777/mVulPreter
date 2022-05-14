std::unique_ptr<base::SharedMemory> GetShmFromMojoHandle(
     mojo::ScopedSharedBufferHandle handle) {
   base::SharedMemoryHandle memory_handle;
   size_t memory_size = 0;
  mojo::UnwrappedSharedMemoryHandleProtection protection;
 
   const MojoResult result = mojo::UnwrapSharedMemoryHandle(
      std::move(handle), &memory_handle, &memory_size, &protection);
   if (result != MOJO_RESULT_OK)
     return nullptr;
   DCHECK_GT(memory_size, 0u);
 
  const bool read_only =
      protection == mojo::UnwrappedSharedMemoryHandleProtection::kReadOnly;
   std::unique_ptr<base::SharedMemory> shm =
      std::make_unique<base::SharedMemory>(memory_handle, read_only);
   if (!shm->Map(memory_size)) {
     DLOG(ERROR) << "Map shared memory failed.";
     return nullptr;
  }
  return shm;
}
