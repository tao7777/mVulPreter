bool SharedMemoryHandleProvider::InitFromMojoHandle(
    mojo::ScopedSharedBufferHandle buffer_handle) {
#if DCHECK_IS_ON()
  DCHECK_EQ(map_ref_count_, 0);
#endif
   DCHECK(!shared_memory_);
 
   base::SharedMemoryHandle memory_handle;
  mojo::UnwrappedSharedMemoryHandleProtection protection;
  const MojoResult result = mojo::UnwrapSharedMemoryHandle(
      std::move(buffer_handle), &memory_handle, &mapped_size_, &protection);
   if (result != MOJO_RESULT_OK)
     return false;
  read_only_flag_ =
      protection == mojo::UnwrappedSharedMemoryHandleProtection::kReadOnly;
   shared_memory_.emplace(memory_handle, read_only_flag_);
   return true;
 }
