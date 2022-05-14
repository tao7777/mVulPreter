void SocketStream::Connect() {
  DCHECK(base::MessageLoop::current())
       << "The current base::MessageLoop must exist";
   DCHECK_EQ(base::MessageLoop::TYPE_IO, base::MessageLoop::current()->type())
       << "The current base::MessageLoop must be TYPE_IO";
  if (context_.get()) {
     context_->ssl_config_service()->GetSSLConfig(&server_ssl_config_);
     proxy_ssl_config_ = server_ssl_config_;
   }
  CheckPrivacyMode();

  DCHECK_EQ(next_state_, STATE_NONE);

  AddRef();  // Released in Finish()
  next_state_ = STATE_BEFORE_CONNECT;
  net_log_.BeginEvent(
      NetLog::TYPE_SOCKET_STREAM_CONNECT,
      NetLog::StringCallback("url", &url_.possibly_invalid_spec()));
  base::MessageLoop::current()->PostTask(
      FROM_HERE, base::Bind(&SocketStream::DoLoop, this, OK));
}
