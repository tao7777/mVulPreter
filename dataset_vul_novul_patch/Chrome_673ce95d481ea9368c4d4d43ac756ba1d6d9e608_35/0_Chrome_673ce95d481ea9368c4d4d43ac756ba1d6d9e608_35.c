    shared_memory_handle(const gfx::GpuMemoryBufferHandle& handle) {
  if (handle.type != gfx::SHARED_MEMORY_BUFFER &&
       handle.type != gfx::DXGI_SHARED_HANDLE &&
       handle.type != gfx::ANDROID_HARDWARE_BUFFER)
     return mojo::ScopedSharedBufferHandle();
  return mojo::WrapSharedMemoryHandle(
      handle.handle, handle.handle.GetSize(),
      mojo::UnwrappedSharedMemoryHandleProtection::kReadWrite);
 }
