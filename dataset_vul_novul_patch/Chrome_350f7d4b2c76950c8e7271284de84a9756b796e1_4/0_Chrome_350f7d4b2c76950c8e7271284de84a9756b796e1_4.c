P2PQuicTransportImpl::P2PQuicTransportImpl(
    P2PQuicTransportConfig p2p_transport_config,
    std::unique_ptr<net::QuicChromiumConnectionHelper> helper,
    std::unique_ptr<quic::QuicConnection> connection,
    const quic::QuicConfig& quic_config,
    quic::QuicClock* clock)
    : quic::QuicSession(connection.get(),
                        nullptr /* visitor */,
                        quic_config,
                        quic::CurrentSupportedVersions()),
      helper_(std::move(helper)),
      connection_(std::move(connection)),
      perspective_(p2p_transport_config.is_server
                       ? quic::Perspective::IS_SERVER
                        : quic::Perspective::IS_CLIENT),
       packet_transport_(p2p_transport_config.packet_transport),
       delegate_(p2p_transport_config.delegate),
      clock_(clock),
      stream_write_buffer_size_(p2p_transport_config.stream_write_buffer_size) {
   DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
   DCHECK(delegate_);
   DCHECK(clock_);
   DCHECK(packet_transport_);
  DCHECK_GT(stream_write_buffer_size_, 0u);
   DCHECK_GT(p2p_transport_config.certificates.size(), 0u);
   if (p2p_transport_config.can_respond_to_crypto_handshake) {
     InitializeCryptoStream();
  }
  certificate_ = p2p_transport_config.certificates[0];
  packet_transport_->SetReceiveDelegate(this);
}
