 bool WebSocketHandshake::ProcessHeaders(const HttpResponseHeaders& headers) {
  std::string value;
  if (!GetSingleHeader(headers, "upgrade", &value) ||
      value != "WebSocket")
    return false;

  if (!GetSingleHeader(headers, "connection", &value) ||
      !LowerCaseEqualsASCII(value, "upgrade"))
    return false;

  if (!GetSingleHeader(headers, "sec-websocket-origin", &ws_origin_))
     return false;
 
  if (!GetSingleHeader(headers, "sec-websocket-location", &ws_location_))
     return false;
 
   if (!protocol_.empty()
      && !GetSingleHeader(headers, "sec-websocket-protocol", &ws_protocol_))
     return false;
   return true;
 }
