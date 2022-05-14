void WebSocket::OnConnected(SocketStream* socket_stream,
                            int max_pending_send_allowed) {
  DCHECK(socket_stream == socket_stream_);
  max_pending_send_allowed_ = max_pending_send_allowed;

  current_read_buf_ = new GrowableIOBuffer();
  current_read_buf_->SetCapacity(max_pending_send_allowed_);
  read_consumed_len_ = 0;
 
   DCHECK(!current_write_buf_);
   DCHECK(!handshake_.get());
  switch (request_->version()) {
    case DEFAULT_VERSION:
      handshake_.reset(new WebSocketHandshake(
          request_->url(), request_->origin(), request_->location(),
          request_->protocol()));
      break;
    case DRAFT75:
      handshake_.reset(new WebSocketHandshakeDraft75(
          request_->url(), request_->origin(), request_->location(),
          request_->protocol()));
      break;
    default:
      NOTREACHED() << "Unexpected protocol version:" << request_->version();
  }
 
   const std::string msg = handshake_->CreateClientHandshakeMessage();
   IOBufferWithSize* buf = new IOBufferWithSize(msg.size());
  memcpy(buf->data(), msg.data(), msg.size());
  pending_write_bufs_.push_back(buf);
  origin_loop_->PostTask(FROM_HERE,
                         NewRunnableMethod(this, &WebSocket::SendPending));
}
