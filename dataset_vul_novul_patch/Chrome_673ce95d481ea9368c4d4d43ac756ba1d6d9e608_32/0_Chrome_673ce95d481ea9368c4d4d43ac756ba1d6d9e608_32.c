MojoResult Core::WrapPlatformSharedBufferHandle(
    const MojoPlatformHandle* platform_handle,
    size_t size,
    const MojoSharedBufferGuid* guid,
    MojoPlatformSharedBufferHandleFlags flags,
    MojoHandle* mojo_handle) {
  DCHECK(size);
  ScopedPlatformHandle handle;
  MojoResult result =
      MojoPlatformHandleToScopedPlatformHandle(platform_handle, &handle);
  if (result != MOJO_RESULT_OK)
    return result;
 
   base::UnguessableToken token =
       base::UnguessableToken::Deserialize(guid->high, guid->low);
  const bool read_only =
      flags & MOJO_PLATFORM_SHARED_BUFFER_HANDLE_FLAG_HANDLE_IS_READ_ONLY;
   scoped_refptr<PlatformSharedBuffer> platform_buffer =
       PlatformSharedBuffer::CreateFromPlatformHandle(size, read_only, token,
                                                      std::move(handle));
  if (!platform_buffer)
    return MOJO_RESULT_UNKNOWN;

  scoped_refptr<SharedBufferDispatcher> dispatcher;
  result = SharedBufferDispatcher::CreateFromPlatformSharedBuffer(
      platform_buffer, &dispatcher);
  if (result != MOJO_RESULT_OK)
    return result;

  MojoHandle h = AddDispatcher(dispatcher);
  if (h == MOJO_HANDLE_INVALID) {
    dispatcher->Close();
    return MOJO_RESULT_RESOURCE_EXHAUSTED;
  }

  *mojo_handle = h;
  return MOJO_RESULT_OK;
}
