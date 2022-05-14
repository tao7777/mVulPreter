void GpuCommandBufferStub::OnCreateTransferBuffer(int32 size,
                                                   int32 id_request,
                                                   IPC::Message* reply_message) {
   TRACE_EVENT0("gpu", "GpuCommandBufferStub::OnCreateTransferBuffer");
  if (command_buffer_.get()) {
    int32 id = command_buffer_->CreateTransferBuffer(size, id_request);
    GpuCommandBufferMsg_CreateTransferBuffer::WriteReplyParams(
        reply_message, id);
  } else {
    reply_message->set_reply_error();
  }
  Send(reply_message);
}
