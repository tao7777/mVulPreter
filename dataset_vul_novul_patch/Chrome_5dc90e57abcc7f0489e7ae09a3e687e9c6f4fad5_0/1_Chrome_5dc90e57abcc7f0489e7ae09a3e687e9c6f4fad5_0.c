 bool WebSocketJob::SendDataInternal(const char* data, int length) {
   if (spdy_websocket_stream_.get())
     return ERR_IO_PENDING == spdy_websocket_stream_->SendData(data, length);
  return socket_->SendData(data, length);
 }
