void GpuCommandBufferStub::OnRegisterTransferBuffer(
    base::SharedMemoryHandle transfer_buffer,
     size_t size,
     int32 id_request,
     IPC::Message* reply_message) {
#if defined(OS_WIN)
  base::SharedMemory shared_memory(transfer_buffer,
                                   false,
                                   channel_->renderer_process());
#else
#endif
 
   if (command_buffer_.get()) {
     int32 id = command_buffer_->RegisterTransferBuffer(&shared_memory,
                                                       size,
                                                       id_request);
    GpuCommandBufferMsg_RegisterTransferBuffer::WriteReplyParams(reply_message,
                                                                 id);
  } else {
    reply_message->set_reply_error();
  }

  Send(reply_message);
}
