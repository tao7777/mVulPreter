 void GpuCommandBufferStub::OnGetTransferBuffer(
     int32 id,
     IPC::Message* reply_message) {
   if (command_buffer_.get()) {
     base::SharedMemoryHandle transfer_buffer = base::SharedMemoryHandle();
     uint32 size = 0;
 
     gpu::Buffer buffer = command_buffer_->GetTransferBuffer(id);
     if (buffer.shared_memory) {
#if defined(OS_WIN)
      transfer_buffer = NULL;
      sandbox::BrokerDuplicateHandle(buffer.shared_memory->handle(),
          channel_->renderer_pid(), &transfer_buffer, FILE_MAP_READ |
          FILE_MAP_WRITE, 0);
      CHECK(transfer_buffer != NULL);
#else
      buffer.shared_memory->ShareToProcess(channel_->renderer_pid(),
                                            &transfer_buffer);
#endif
       size = buffer.size;
     }
 
    GpuCommandBufferMsg_GetTransferBuffer::WriteReplyParams(reply_message,
                                                            transfer_buffer,
                                                            size);
  } else {
    reply_message->set_reply_error();
  }
  Send(reply_message);
}
