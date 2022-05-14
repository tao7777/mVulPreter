MojoResult UnwrapSharedMemoryHandle(ScopedSharedBufferHandle handle,
MojoResult UnwrapSharedMemoryHandle(
    ScopedSharedBufferHandle handle,
    base::SharedMemoryHandle* memory_handle,
    size_t* size,
    UnwrappedSharedMemoryHandleProtection* protection) {
   if (!handle.is_valid())
     return MOJO_RESULT_INVALID_ARGUMENT;
   MojoPlatformHandle platform_handle;
  platform_handle.struct_size = sizeof(MojoPlatformHandle);

  MojoPlatformSharedBufferHandleFlags flags;
  size_t num_bytes;
  MojoSharedBufferGuid mojo_guid;
  MojoResult result = MojoUnwrapPlatformSharedBufferHandle(
      handle.release().value(), &platform_handle, &num_bytes, &mojo_guid,
      &flags);
  if (result != MOJO_RESULT_OK)
    return result;

   if (size)
     *size = num_bytes;
 
  if (protection) {
    *protection =
        flags & MOJO_PLATFORM_SHARED_BUFFER_HANDLE_FLAG_HANDLE_IS_READ_ONLY
            ? UnwrappedSharedMemoryHandleProtection::kReadOnly
            : UnwrappedSharedMemoryHandleProtection::kReadWrite;
  }
 
   base::UnguessableToken guid =
       base::UnguessableToken::Deserialize(mojo_guid.high, mojo_guid.low);
#if defined(OS_MACOSX) && !defined(OS_IOS)
  DCHECK_EQ(platform_handle.type, MOJO_PLATFORM_HANDLE_TYPE_MACH_PORT);
  *memory_handle = base::SharedMemoryHandle(
      static_cast<mach_port_t>(platform_handle.value), num_bytes, guid);
#elif defined(OS_FUCHSIA)
  DCHECK_EQ(platform_handle.type, MOJO_PLATFORM_HANDLE_TYPE_FUCHSIA_HANDLE);
  *memory_handle = base::SharedMemoryHandle(
      static_cast<zx_handle_t>(platform_handle.value), num_bytes, guid);
#elif defined(OS_POSIX)
  DCHECK_EQ(platform_handle.type, MOJO_PLATFORM_HANDLE_TYPE_FILE_DESCRIPTOR);
  *memory_handle = base::SharedMemoryHandle(
      base::FileDescriptor(static_cast<int>(platform_handle.value), false),
      num_bytes, guid);
#elif defined(OS_WIN)
  DCHECK_EQ(platform_handle.type, MOJO_PLATFORM_HANDLE_TYPE_WINDOWS_HANDLE);
  *memory_handle = base::SharedMemoryHandle(
      reinterpret_cast<HANDLE>(platform_handle.value), num_bytes, guid);
#endif

  return MOJO_RESULT_OK;
}
