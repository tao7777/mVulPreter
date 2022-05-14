 void SocketStream::set_context(URLRequestContext* context) {
  const URLRequestContext* prev_context = context_;
 
  context_ = context;
 
   if (prev_context != context) {
     if (prev_context && pac_request_) {
      prev_context->proxy_service()->CancelPacRequest(pac_request_);
      pac_request_ = NULL;
    }

    net_log_.EndEvent(NetLog::TYPE_REQUEST_ALIVE);
    net_log_ = BoundNetLog();

    if (context) {
      net_log_ = BoundNetLog::Make(
          context->net_log(),
          NetLog::SOURCE_SOCKET_STREAM);

      net_log_.BeginEvent(NetLog::TYPE_REQUEST_ALIVE);
    }
  }
 }
