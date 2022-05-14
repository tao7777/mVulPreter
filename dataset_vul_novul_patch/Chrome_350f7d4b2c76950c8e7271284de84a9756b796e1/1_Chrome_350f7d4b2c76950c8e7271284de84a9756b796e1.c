void P2PQuicStreamImpl::Finish() {
  DCHECK(!fin_sent());
  quic::QuicStream::WriteOrBufferData("", /*fin=*/true, nullptr);
 }
