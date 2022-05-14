int HttpProxyClientSocket::DoReadHeadersComplete(int result) {
  if (result < 0)
    return result;

  if (response_.headers->GetParsedHttpVersion() < HttpVersion(1, 0))
    return ERR_TUNNEL_CONNECTION_FAILED;

  net_log_.AddEvent(
      NetLog::TYPE_HTTP_TRANSACTION_READ_TUNNEL_RESPONSE_HEADERS,
      base::Bind(&HttpResponseHeaders::NetLogCallback, response_.headers));

  if (proxy_delegate_) {
    proxy_delegate_->OnTunnelHeadersReceived(
        HostPortPair::FromURL(request_.url),
        proxy_server_,
        *response_.headers);
  }

  switch (response_.headers->response_code()) {
    case 200:  // OK
      if (http_stream_parser_->IsMoreDataBuffered())
        return ERR_TUNNEL_CONNECTION_FAILED;

      next_state_ = STATE_DONE;
      return OK;


    case 302:  // Found / Moved Temporarily
      if (is_https_proxy_ && SanitizeProxyRedirect(&response_, request_.url)) {
        bool is_connection_reused = http_stream_parser_->IsConnectionReused();
        redirect_has_load_timing_info_ =
            transport_->GetLoadTimingInfo(
                is_connection_reused, &redirect_load_timing_info_);
        transport_.reset();
        http_stream_parser_.reset();
        return ERR_HTTPS_PROXY_TUNNEL_RESPONSE;
       }
 
      LogBlockedTunnelResponse();
      return ERR_TUNNEL_CONNECTION_FAILED;
 
     case 407:  // Proxy Authentication Required
       return HandleProxyAuthChallenge(auth_.get(), &response_, net_log_);
 
     default:
      LogBlockedTunnelResponse();
      return ERR_TUNNEL_CONNECTION_FAILED;
  }
}
