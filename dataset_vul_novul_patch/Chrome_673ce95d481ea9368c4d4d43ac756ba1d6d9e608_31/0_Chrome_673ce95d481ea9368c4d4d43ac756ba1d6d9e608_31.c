MojoResult Core::UnwrapPlatformSharedBufferHandle(
    MojoHandle mojo_handle,
    MojoPlatformHandle* platform_handle,
    size_t* size,
    MojoSharedBufferGuid* guid,
    MojoPlatformSharedBufferHandleFlags* flags) {
  scoped_refptr<Dispatcher> dispatcher;
  MojoResult result = MOJO_RESULT_OK;
  {
    base::AutoLock lock(handles_->GetLock());
    result = handles_->GetAndRemoveDispatcher(mojo_handle, &dispatcher);
    if (result != MOJO_RESULT_OK)
      return result;
  }

  if (dispatcher->GetType() != Dispatcher::Type::SHARED_BUFFER) {
    dispatcher->Close();
    return MOJO_RESULT_INVALID_ARGUMENT;
  }

  SharedBufferDispatcher* shm_dispatcher =
      static_cast<SharedBufferDispatcher*>(dispatcher.get());
  scoped_refptr<PlatformSharedBuffer> platform_shared_buffer =
      shm_dispatcher->PassPlatformSharedBuffer();
  DCHECK(platform_shared_buffer);

  DCHECK(size);
  *size = platform_shared_buffer->GetNumBytes();

  base::UnguessableToken token = platform_shared_buffer->GetGUID();
  guid->high = token.GetHighForSerialization();
  guid->low = token.GetLowForSerialization();

   DCHECK(flags);
   *flags = MOJO_PLATFORM_SHARED_BUFFER_HANDLE_FLAG_NONE;
   if (platform_shared_buffer->IsReadOnly())
    *flags |= MOJO_PLATFORM_SHARED_BUFFER_HANDLE_FLAG_HANDLE_IS_READ_ONLY;
 
   ScopedPlatformHandle handle = platform_shared_buffer->PassPlatformHandle();
   return ScopedPlatformHandleToMojoPlatformHandle(std::move(handle),
                                                  platform_handle);
}
