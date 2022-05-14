void MojoAudioInputIPC::StreamCreated(
    media::mojom::AudioInputStreamPtr stream,
    media::mojom::AudioInputStreamClientRequest stream_client_request,
    mojo::ScopedSharedBufferHandle shared_memory,
    mojo::ScopedHandle socket,
    bool initially_muted) {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  DCHECK(delegate_);
  DCHECK(socket.is_valid());
  DCHECK(shared_memory.is_valid());
  DCHECK(!stream_);
  DCHECK(!stream_client_binding_.is_bound());
  stream_ = std::move(stream);
  stream_client_binding_.Bind(std::move(stream_client_request));

  base::PlatformFile socket_handle;
  auto result = mojo::UnwrapPlatformFile(std::move(socket), &socket_handle);
   DCHECK_EQ(result, MOJO_RESULT_OK);
 
   base::SharedMemoryHandle memory_handle;
  bool read_only = true;
   result = mojo::UnwrapSharedMemoryHandle(std::move(shared_memory),
                                          &memory_handle, nullptr, &read_only);
   DCHECK_EQ(result, MOJO_RESULT_OK);
  DCHECK(read_only);
 
   delegate_->OnStreamCreated(memory_handle, socket_handle, initially_muted);
 }
