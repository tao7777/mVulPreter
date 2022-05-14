 ScopedSharedBufferHandle WrapSharedMemoryHandle(
     const base::SharedMemoryHandle& memory_handle,
     size_t size,
    UnwrappedSharedMemoryHandleProtection protection) {
   if (!memory_handle.IsValid())
     return ScopedSharedBufferHandle();
   MojoPlatformHandle platform_handle;
  platform_handle.struct_size = sizeof(MojoPlatformHandle);
  platform_handle.type = kPlatformSharedBufferHandleType;
#if defined(OS_MACOSX) && !defined(OS_IOS)
  platform_handle.value =
      static_cast<uint64_t>(memory_handle.GetMemoryObject());
#else
  platform_handle.value =
      PlatformHandleValueFromPlatformFile(memory_handle.GetHandle());
#endif
 
   MojoPlatformSharedBufferHandleFlags flags =
       MOJO_PLATFORM_SHARED_BUFFER_HANDLE_FLAG_NONE;
  if (protection == UnwrappedSharedMemoryHandleProtection::kReadOnly)
    flags |= MOJO_PLATFORM_SHARED_BUFFER_HANDLE_FLAG_HANDLE_IS_READ_ONLY;
 
   MojoSharedBufferGuid guid;
   guid.high = memory_handle.GetGUID().GetHighForSerialization();
  guid.low = memory_handle.GetGUID().GetLowForSerialization();
  MojoHandle mojo_handle;
  MojoResult result = MojoWrapPlatformSharedBufferHandle(
      &platform_handle, size, &guid, flags, &mojo_handle);
  CHECK_EQ(result, MOJO_RESULT_OK);

   return ScopedSharedBufferHandle(SharedBufferHandle(mojo_handle));
 }
