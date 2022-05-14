 void P2PQuicStreamImpl::OnStreamReset(const quic::QuicRstStreamFrame& frame) {
  quic::QuicStream::OnStreamReset(frame);
  delegate_->OnRemoteReset();
}
