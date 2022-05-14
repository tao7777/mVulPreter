 mojo::ScopedSharedBufferHandle GamepadProvider::GetSharedBufferHandle() {
  return mojo::WrapSharedMemoryHandle(
      gamepad_shared_buffer_->shared_memory()->GetReadOnlyHandle(),
      sizeof(GamepadHardwareBuffer),
      mojo::UnwrappedSharedMemoryHandleProtection::kReadOnly);
 }
