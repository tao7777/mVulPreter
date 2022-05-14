 mojo::ScopedSharedBufferHandle GamepadProvider::GetSharedBufferHandle() {
  base::SharedMemoryHandle handle = base::SharedMemory::DuplicateHandle(
      gamepad_shared_buffer_->shared_memory()->handle());
  return mojo::WrapSharedMemoryHandle(handle, sizeof(GamepadHardwareBuffer),
                                      true /* read_only */);
 }
