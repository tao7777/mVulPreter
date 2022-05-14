std::string WebSocketHandshake::CreateClientHandshakeMessage() const {
   std::string msg;
   msg = "GET ";
  msg += url_.path();
   if (url_.has_query()) {
    msg += "?";
    msg += url_.query();
   }
  msg += " HTTP/1.1\r\n";
  msg += kUpgradeHeader;
  msg += kConnectionHeader;
  msg += "Host: ";
  msg += StringToLowerASCII(url_.host());
   if (url_.has_port()) {
     bool secure = is_secure();
     int port = url_.EffectiveIntPort();
     if ((!secure &&
          port != kWebSocketPort && port != url_parse::PORT_UNSPECIFIED) ||
         (secure &&
          port != kSecureWebSocketPort && port != url_parse::PORT_UNSPECIFIED)) {
      msg += ":";
      msg += IntToString(port);
     }
   }
  msg += "\r\n";
  msg += "Origin: ";
  msg += StringToLowerASCII(origin_);
  msg += "\r\n";
  if (!protocol_.empty()) {
    msg += "WebSocket-Protocol: ";
    msg += protocol_;
    msg += "\r\n";
  }
  msg += "\r\n";
  return msg;
 }
