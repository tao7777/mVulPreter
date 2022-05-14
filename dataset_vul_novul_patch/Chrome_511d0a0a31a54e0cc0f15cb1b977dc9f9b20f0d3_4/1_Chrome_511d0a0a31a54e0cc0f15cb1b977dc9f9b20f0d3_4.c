int WebSocketHandshake::ReadServerHandshake(const char* data, size_t len) {
  mode_ = MODE_INCOMPLETE;
  if (len < kServerHandshakeHeaderLength) {
    return -1;
  }
  if (!memcmp(data, kServerHandshakeHeader, kServerHandshakeHeaderLength)) {
    mode_ = MODE_NORMAL;
  } else {
    int eoh = HttpUtil::LocateEndOfHeaders(data, len);
    if (eoh < 0)
      return -1;
    return eoh;
  }
  const char* p = data + kServerHandshakeHeaderLength;
  const char* end = data + len + 1;
  if (mode_ == MODE_NORMAL) {
    size_t header_size = end - p;
    if (header_size < kUpgradeHeaderLength)
      return -1;
    if (memcmp(p, kUpgradeHeader, kUpgradeHeaderLength)) {
      mode_ = MODE_FAILED;
      DLOG(INFO) << "Bad Upgrade Header "
                 << std::string(p, kUpgradeHeaderLength);
      return p - data;
    }
    p += kUpgradeHeaderLength;
    header_size = end - p;
    if (header_size < kConnectionHeaderLength)
      return -1;
    if (memcmp(p, kConnectionHeader, kConnectionHeaderLength)) {
      mode_ = MODE_FAILED;
      DLOG(INFO) << "Bad Connection Header "
                 << std::string(p, kConnectionHeaderLength);
      return p - data;
    }
    p += kConnectionHeaderLength;
  }
  int eoh = HttpUtil::LocateEndOfHeaders(data, len);
  if (eoh == -1)
    return eoh;
  scoped_refptr<HttpResponseHeaders> headers(
      new HttpResponseHeaders(HttpUtil::AssembleRawHeaders(data, eoh)));
  if (!ProcessHeaders(*headers)) {
    DLOG(INFO) << "Process Headers failed: "
               << std::string(data, eoh);
    mode_ = MODE_FAILED;
  }
  switch (mode_) {
    case MODE_NORMAL:
      if (CheckResponseHeaders()) {
        mode_ = MODE_CONNECTED;
      } else {
        mode_ = MODE_FAILED;
      }
      break;
    default:
      mode_ = MODE_FAILED;
      break;
  }
  return eoh;
}
