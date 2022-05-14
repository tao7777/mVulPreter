void MojoAudioOutputStream::OnStreamCreated(
    int stream_id,
    const base::SharedMemory* shared_memory,
    std::unique_ptr<base::CancelableSyncSocket> foreign_socket) {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  DCHECK(stream_created_callback_);
  DCHECK(shared_memory);
  DCHECK(foreign_socket);

  base::SharedMemoryHandle foreign_memory_handle =
      base::SharedMemory::DuplicateHandle(shared_memory->handle());
  if (!base::SharedMemory::IsHandleValid(foreign_memory_handle)) {
    OnStreamError(/*not used*/ 0);
    return;
   }
 
   mojo::ScopedSharedBufferHandle buffer_handle = mojo::WrapSharedMemoryHandle(
      foreign_memory_handle, shared_memory->requested_size(),
      mojo::UnwrappedSharedMemoryHandleProtection::kReadWrite);
   mojo::ScopedHandle socket_handle =
       mojo::WrapPlatformFile(foreign_socket->Release());
 
  DCHECK(buffer_handle.is_valid());
  DCHECK(socket_handle.is_valid());

  base::ResetAndReturn(&stream_created_callback_)
      .Run(std::move(buffer_handle), std::move(socket_handle));
}
