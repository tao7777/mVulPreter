std::string WebSocketHandshake::CreateClientHandshakeMessage() const {
std::string WebSocketHandshake::CreateClientHandshakeMessage() {
  if (!parameter_.get()) {
    parameter_.reset(new Parameter);
    parameter_->GenerateKeys();
  }
   std::string msg;

  // WebSocket protocol 4.1 Opening handshake.

   msg = "GET ";
  msg += GetResourceName();
  msg += " HTTP/1.1\r\n";

  std::vector<std::string> fields;

  fields.push_back("Upgrade: WebSocket");
  fields.push_back("Connection: Upgrade");

  fields.push_back("Host: " + GetHostFieldValue());

  fields.push_back("Origin: " + GetOriginFieldValue());

  if (!protocol_.empty())
    fields.push_back("Sec-WebSocket-Protocol: " + protocol_);

  // TODO(ukai): Add cookie if necessary.

  fields.push_back("Sec-WebSocket-Key1: " + parameter_->GetSecWebSocketKey1());
  fields.push_back("Sec-WebSocket-Key2: " + parameter_->GetSecWebSocketKey2());

  std::random_shuffle(fields.begin(), fields.end());

  for (size_t i = 0; i < fields.size(); i++) {
    msg += fields[i] + "\r\n";
  }
  msg += "\r\n";

  msg.append(parameter_->GetKey3());
  return msg;
}

int WebSocketHandshake::ReadServerHandshake(const char* data, size_t len) {
  mode_ = MODE_INCOMPLETE;
  int eoh = HttpUtil::LocateEndOfHeaders(data, len);
  if (eoh < 0)
    return -1;

  scoped_refptr<HttpResponseHeaders> headers(
      new HttpResponseHeaders(HttpUtil::AssembleRawHeaders(data, eoh)));

  if (headers->response_code() != 101) {
    mode_ = MODE_FAILED;
    DLOG(INFO) << "Bad response code: " << headers->response_code();
    return eoh;
  }
  mode_ = MODE_NORMAL;
  if (!ProcessHeaders(*headers) || !CheckResponseHeaders()) {
    DLOG(INFO) << "Process Headers failed: "
               << std::string(data, eoh);
    mode_ = MODE_FAILED;
    return eoh;
  }
  if (len < static_cast<size_t>(eoh + Parameter::kExpectedResponseSize)) {
    mode_ = MODE_INCOMPLETE;
    return -1;
  }
  uint8 expected[Parameter::kExpectedResponseSize];
  parameter_->GetExpectedResponse(expected);
  if (memcmp(&data[eoh], expected, Parameter::kExpectedResponseSize)) {
    mode_ = MODE_FAILED;
    return eoh + Parameter::kExpectedResponseSize;
  }
  mode_ = MODE_CONNECTED;
  return eoh + Parameter::kExpectedResponseSize;
}

std::string WebSocketHandshake::GetResourceName() const {
  std::string resource_name = url_.path();
   if (url_.has_query()) {
    resource_name += "?";
    resource_name += url_.query();
   }
  return resource_name;
}

std::string WebSocketHandshake::GetHostFieldValue() const {
  // url_.host() is expected to be encoded in punnycode here.
  std::string host = StringToLowerASCII(url_.host());
   if (url_.has_port()) {
     bool secure = is_secure();
     int port = url_.EffectiveIntPort();
     if ((!secure &&
          port != kWebSocketPort && port != url_parse::PORT_UNSPECIFIED) ||
         (secure &&
          port != kSecureWebSocketPort && port != url_parse::PORT_UNSPECIFIED)) {
      host += ":";
      host += IntToString(port);
     }
   }
  return host;
}

std::string WebSocketHandshake::GetOriginFieldValue() const {
  return StringToLowerASCII(origin_);
 }
