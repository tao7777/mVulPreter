void MojoAudioOutputIPC::StreamCreated(
    mojo::ScopedSharedBufferHandle shared_memory,
    mojo::ScopedHandle socket) {
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
  DCHECK(delegate_);
  DCHECK(socket.is_valid());
  DCHECK(shared_memory.is_valid());

  base::PlatformFile socket_handle;
  auto result = mojo::UnwrapPlatformFile(std::move(socket), &socket_handle);
   DCHECK_EQ(result, MOJO_RESULT_OK);
 
   base::SharedMemoryHandle memory_handle;
  mojo::UnwrappedSharedMemoryHandleProtection protection;
   size_t memory_length = 0;
   result = mojo::UnwrapSharedMemoryHandle(
      std::move(shared_memory), &memory_handle, &memory_length, &protection);
   DCHECK_EQ(result, MOJO_RESULT_OK);
  DCHECK_EQ(protection,
            mojo::UnwrappedSharedMemoryHandleProtection::kReadWrite);
 
   delegate_->OnStreamCreated(memory_handle, socket_handle);
 }
