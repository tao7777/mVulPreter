 void GpuCommandBufferStub::OnGetTransferBuffer(
     int32 id,
     IPC::Message* reply_message) {
  if (!channel_->renderer_process())
    return;
   if (command_buffer_.get()) {
     base::SharedMemoryHandle transfer_buffer = base::SharedMemoryHandle();
     uint32 size = 0;
 
     gpu::Buffer buffer = command_buffer_->GetTransferBuffer(id);
     if (buffer.shared_memory) {
      buffer.shared_memory->ShareToProcess(channel_->renderer_process(),
                                            &transfer_buffer);
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
