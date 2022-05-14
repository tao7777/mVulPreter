P2PQuicStreamImpl* P2PQuicTransportImpl::CreateStreamInternal(
    quic::QuicStreamId id) {
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
   DCHECK(crypto_stream_);
   DCHECK(IsEncryptionEstablished());
   DCHECK(!IsClosed());
  return new P2PQuicStreamImpl(id, this, stream_write_buffer_size_);
 }
