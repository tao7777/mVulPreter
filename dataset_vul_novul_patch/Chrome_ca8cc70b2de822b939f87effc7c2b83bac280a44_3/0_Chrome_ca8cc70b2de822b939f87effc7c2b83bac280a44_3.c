 void SocketStream::DoLoop(int result) {
  if (!context_)
     next_state_ = STATE_CLOSE;
 
   if (next_state_ == STATE_NONE)
    return;

  do {
    State state = next_state_;
    next_state_ = STATE_NONE;
    switch (state) {
      case STATE_BEFORE_CONNECT:
        DCHECK_EQ(OK, result);
        result = DoBeforeConnect();
        break;
      case STATE_BEFORE_CONNECT_COMPLETE:
        result = DoBeforeConnectComplete(result);
        break;
      case STATE_RESOLVE_PROXY:
        DCHECK_EQ(OK, result);
        result = DoResolveProxy();
        break;
      case STATE_RESOLVE_PROXY_COMPLETE:
        result = DoResolveProxyComplete(result);
        break;
      case STATE_RESOLVE_HOST:
        DCHECK_EQ(OK, result);
        result = DoResolveHost();
        break;
      case STATE_RESOLVE_HOST_COMPLETE:
        result = DoResolveHostComplete(result);
        break;
      case STATE_RESOLVE_PROTOCOL:
        result = DoResolveProtocol(result);
        break;
      case STATE_RESOLVE_PROTOCOL_COMPLETE:
        result = DoResolveProtocolComplete(result);
        break;
      case STATE_TCP_CONNECT:
        result = DoTcpConnect(result);
        break;
      case STATE_TCP_CONNECT_COMPLETE:
        result = DoTcpConnectComplete(result);
        break;
      case STATE_GENERATE_PROXY_AUTH_TOKEN:
        result = DoGenerateProxyAuthToken();
        break;
      case STATE_GENERATE_PROXY_AUTH_TOKEN_COMPLETE:
        result = DoGenerateProxyAuthTokenComplete(result);
        break;
      case STATE_WRITE_TUNNEL_HEADERS:
        DCHECK_EQ(OK, result);
        result = DoWriteTunnelHeaders();
        break;
      case STATE_WRITE_TUNNEL_HEADERS_COMPLETE:
        result = DoWriteTunnelHeadersComplete(result);
        break;
      case STATE_READ_TUNNEL_HEADERS:
        DCHECK_EQ(OK, result);
        result = DoReadTunnelHeaders();
        break;
      case STATE_READ_TUNNEL_HEADERS_COMPLETE:
        result = DoReadTunnelHeadersComplete(result);
        break;
      case STATE_SOCKS_CONNECT:
        DCHECK_EQ(OK, result);
        result = DoSOCKSConnect();
        break;
      case STATE_SOCKS_CONNECT_COMPLETE:
        result = DoSOCKSConnectComplete(result);
        break;
      case STATE_SECURE_PROXY_CONNECT:
        DCHECK_EQ(OK, result);
        result = DoSecureProxyConnect();
        break;
      case STATE_SECURE_PROXY_CONNECT_COMPLETE:
        result = DoSecureProxyConnectComplete(result);
        break;
      case STATE_SECURE_PROXY_HANDLE_CERT_ERROR:
        result = DoSecureProxyHandleCertError(result);
        break;
      case STATE_SECURE_PROXY_HANDLE_CERT_ERROR_COMPLETE:
        result = DoSecureProxyHandleCertErrorComplete(result);
        break;
      case STATE_SSL_CONNECT:
        DCHECK_EQ(OK, result);
        result = DoSSLConnect();
        break;
      case STATE_SSL_CONNECT_COMPLETE:
        result = DoSSLConnectComplete(result);
        break;
      case STATE_SSL_HANDLE_CERT_ERROR:
        result = DoSSLHandleCertError(result);
        break;
      case STATE_SSL_HANDLE_CERT_ERROR_COMPLETE:
        result = DoSSLHandleCertErrorComplete(result);
        break;
      case STATE_READ_WRITE:
        result = DoReadWrite(result);
        break;
      case STATE_AUTH_REQUIRED:
        Finish(result);
        return;
      case STATE_CLOSE:
        DCHECK_LE(result, OK);
        Finish(result);
        return;
      default:
        NOTREACHED() << "bad state " << state;
        Finish(result);
        return;
    }
    if (state == STATE_RESOLVE_PROTOCOL && result == ERR_PROTOCOL_SWITCHED)
      continue;
    if (state != STATE_READ_WRITE && result < ERR_IO_PENDING) {
      net_log_.EndEventWithNetErrorCode(
          NetLog::TYPE_SOCKET_STREAM_CONNECT, result);
    }
  } while (result != ERR_IO_PENDING);
}
