void QuicClientPromisedInfo::OnPromiseHeaders(const SpdyHeaderBlock& headers) {
   SpdyHeaderBlock::const_iterator it = headers.find(kHttp2MethodHeader);
  if (it == headers.end()) {
    QUIC_DVLOG(1) << "Promise for stream " << id_ << " has no method";
    Reset(QUIC_INVALID_PROMISE_METHOD);
    return;
  }
   if (!(it->second == "GET" || it->second == "HEAD")) {
     QUIC_DVLOG(1) << "Promise for stream " << id_ << " has invalid method "
                   << it->second;
    Reset(QUIC_INVALID_PROMISE_METHOD);
    return;
  }
  if (!SpdyUtils::UrlIsValid(headers)) {
    QUIC_DVLOG(1) << "Promise for stream " << id_ << " has invalid URL "
                  << url_;
    Reset(QUIC_INVALID_PROMISE_URL);
    return;
  }
  if (!session_->IsAuthorized(SpdyUtils::GetHostNameFromHeaderBlock(headers))) {
    Reset(QUIC_UNAUTHORIZED_PROMISE_URL);
    return;
  }
  request_headers_.reset(new SpdyHeaderBlock(headers.Clone()));
}
