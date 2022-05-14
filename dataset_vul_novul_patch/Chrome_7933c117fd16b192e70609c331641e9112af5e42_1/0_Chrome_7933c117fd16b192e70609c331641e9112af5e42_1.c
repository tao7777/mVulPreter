int SpdyProxyClientSocket::DoReadReplyComplete(int result) {

  if (result < 0)
    return result;

  if (response_.headers->GetParsedHttpVersion() < HttpVersion(1, 0))
    return ERR_TUNNEL_CONNECTION_FAILED;

  net_log_.AddEvent(
      NetLog::TYPE_HTTP_TRANSACTION_READ_TUNNEL_RESPONSE_HEADERS,
      base::Bind(&HttpResponseHeaders::NetLogCallback, response_.headers));

  switch (response_.headers->response_code()) {
    case 200:  // OK
      next_state_ = STATE_OPEN;
      return OK;

     case 302:  // Found / Moved Temporarily
      if (!SanitizeProxyRedirect(&response_)) {
         LogBlockedTunnelResponse();
         return ERR_TUNNEL_CONNECTION_FAILED;
       }
 
      redirect_has_load_timing_info_ =
          spdy_stream_->GetLoadTimingInfo(&redirect_load_timing_info_);
      // Note that this triggers a RST_STREAM_CANCEL.
      spdy_stream_->DetachDelegate();
      next_state_ = STATE_DISCONNECTED;
      return ERR_HTTPS_PROXY_TUNNEL_RESPONSE;

     case 407:  // Proxy Authentication Required
       next_state_ = STATE_OPEN;
      if (!SanitizeProxyAuth(&response_)) {
        LogBlockedTunnelResponse();
        return ERR_TUNNEL_CONNECTION_FAILED;
      }
       return HandleProxyAuthChallenge(auth_.get(), &response_, net_log_);
 
     default:
      LogBlockedTunnelResponse();
      return ERR_TUNNEL_CONNECTION_FAILED;
  }
}
