void WebSocketJob::OnSentData(SocketStream* socket, int amount_sent) {
  DCHECK_NE(INITIALIZED, state_);
  if (state_ == CLOSED)
    return;
  if (state_ == CONNECTING) {
    OnSentHandshakeRequest(socket, amount_sent);
    return;
  }
  if (delegate_) {
    DCHECK(state_ == OPEN || state_ == CLOSING);
    DCHECK_GT(amount_sent, 0);
    DCHECK(current_buffer_);
    current_buffer_->DidConsume(amount_sent);
    if (current_buffer_->BytesRemaining() > 0)
      return;

    amount_sent = send_frame_handler_->GetOriginalBufferSize();
     DCHECK_GT(amount_sent, 0);
     current_buffer_ = NULL;
     send_frame_handler_->ReleaseCurrentBuffer();
    if (method_factory_.empty()) {
      MessageLoopForIO::current()->PostTask(
          FROM_HERE,
          method_factory_.NewRunnableMethod(&WebSocketJob::SendPending));
    }
     delegate_->OnSentData(socket, amount_sent);
   }
 }
