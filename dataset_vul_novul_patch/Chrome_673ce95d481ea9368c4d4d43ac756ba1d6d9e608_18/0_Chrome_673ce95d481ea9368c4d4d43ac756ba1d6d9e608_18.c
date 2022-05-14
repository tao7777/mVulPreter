SharedMemoryHandleProvider::GetHandleForInterProcessTransit(bool read_only) {
  if (read_only_flag_ && !read_only) {
     NOTREACHED();
     return mojo::ScopedSharedBufferHandle();
   }
  // TODO(https://crbug.com/803136): This does not actually obey |read_only| in
  // any capacity because it uses DuplicateHandle. In order to properly obey
  // |read_only| (when true), we need to use |SharedMemory::GetReadOnlyHandle()|
  // but that is not possible. With the base::SharedMemory API and this
  // SharedMemoryHandleProvider API as they are today, it isn't possible to know
  // whether |shared_memory_| even supports read-only duplication. Note that
  // changing |kReadWrite| to |kReadOnly| does NOT affect the ability to map
  // the handle read-write.
   return mojo::WrapSharedMemoryHandle(
       base::SharedMemory::DuplicateHandle(shared_memory_->handle()),
      mapped_size_, mojo::UnwrappedSharedMemoryHandleProtection::kReadWrite);
 }
