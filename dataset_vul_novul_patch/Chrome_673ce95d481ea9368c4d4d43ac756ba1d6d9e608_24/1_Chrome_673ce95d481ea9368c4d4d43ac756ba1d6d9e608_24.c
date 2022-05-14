    memory_handle(const media::BitstreamBuffer& input) {
  base::SharedMemoryHandle input_handle =
      base::SharedMemory::DuplicateHandle(input.handle());
  if (!base::SharedMemory::IsHandleValid(input_handle)) {
     DLOG(ERROR) << "Failed to duplicate handle of BitstreamBuffer";
     return mojo::ScopedSharedBufferHandle();
   }
  return mojo::WrapSharedMemoryHandle(input_handle, input.size(),
                                      true /* read_only */);
 }
