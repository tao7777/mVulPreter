 bool WebSocketHandshake::ProcessHeaders(const HttpResponseHeaders& headers) {
  if (!GetSingleHeader(headers, "websocket-origin", &ws_origin_))
     return false;
 
  if (!GetSingleHeader(headers, "websocket-location", &ws_location_))
     return false;
 
   if (!protocol_.empty()
      && !GetSingleHeader(headers, "websocket-protocol", &ws_protocol_))
     return false;
   return true;
 }
