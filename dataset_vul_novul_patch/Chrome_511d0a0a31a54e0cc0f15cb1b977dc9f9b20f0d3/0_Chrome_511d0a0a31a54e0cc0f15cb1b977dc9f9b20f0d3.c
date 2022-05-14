net::WebSocket* WebSocketExperimentTask::Context::CreateWebSocket(
    const Config& config, net::WebSocketDelegate* delegate) {
  URLRequestContextGetter* getter =
      Profile::GetDefaultRequestContext();
  if (!getter)
    return NULL;
  net::WebSocket::Request* request(
      new net::WebSocket::Request(config.url,
                                   config.ws_protocol,
                                   config.ws_origin,
                                   config.ws_location,
                                  net::WebSocket::DRAFT75,
                                   getter->GetURLRequestContext()));
   return new net::WebSocket(request, delegate);
 }
