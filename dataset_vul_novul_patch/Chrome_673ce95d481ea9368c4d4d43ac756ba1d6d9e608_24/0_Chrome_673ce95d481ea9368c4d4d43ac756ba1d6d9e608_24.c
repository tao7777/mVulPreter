    memory_handle(const media::BitstreamBuffer& input) {
  base::SharedMemoryHandle input_handle =
      base::SharedMemory::DuplicateHandle(input.handle());
  if (!base::SharedMemory::IsHandleValid(input_handle)) {
     DLOG(ERROR) << "Failed to duplicate handle of BitstreamBuffer";
     return mojo::ScopedSharedBufferHandle();
   }

  // TODO(https://crbug.com/793446): Update this to |kReadOnly| protection once
  // BitstreamBuffer can guarantee that its handle() field always corresponds to
  // a read-only SharedMemoryHandle.
  return mojo::WrapSharedMemoryHandle(
      input_handle, input.size(),
      mojo::UnwrappedSharedMemoryHandleProtection::kReadWrite);
 }
