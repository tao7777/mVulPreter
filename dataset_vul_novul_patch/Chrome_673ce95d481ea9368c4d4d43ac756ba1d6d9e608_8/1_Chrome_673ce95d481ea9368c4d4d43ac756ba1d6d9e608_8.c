std::unique_ptr<base::SharedMemory> GetShmFromMojoHandle(
     mojo::ScopedSharedBufferHandle handle) {
   base::SharedMemoryHandle memory_handle;
   size_t memory_size = 0;
  bool read_only_flag = false;
 
   const MojoResult result = mojo::UnwrapSharedMemoryHandle(
      std::move(handle), &memory_handle, &memory_size, &read_only_flag);
   if (result != MOJO_RESULT_OK)
     return nullptr;
   DCHECK_GT(memory_size, 0u);
 
   std::unique_ptr<base::SharedMemory> shm =
      std::make_unique<base::SharedMemory>(memory_handle, read_only_flag);
   if (!shm->Map(memory_size)) {
     DLOG(ERROR) << "Map shared memory failed.";
     return nullptr;
  }
  return shm;
}
