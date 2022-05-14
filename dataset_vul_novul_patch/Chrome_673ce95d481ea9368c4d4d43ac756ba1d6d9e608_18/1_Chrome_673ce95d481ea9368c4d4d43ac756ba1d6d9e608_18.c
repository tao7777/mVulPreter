SharedMemoryHandleProvider::GetHandleForInterProcessTransit(bool read_only) {
  if (read_only_flag_ && !read_only) {
     NOTREACHED();
     return mojo::ScopedSharedBufferHandle();
   }
   return mojo::WrapSharedMemoryHandle(
       base::SharedMemory::DuplicateHandle(shared_memory_->handle()),
      mapped_size_, read_only);
 }
