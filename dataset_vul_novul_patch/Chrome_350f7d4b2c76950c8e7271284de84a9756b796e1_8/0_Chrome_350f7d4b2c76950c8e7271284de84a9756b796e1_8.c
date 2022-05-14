void QuicTransportHost::Initialize(
    IceTransportHost* ice_transport_host,
    quic::Perspective perspective,
    const std::vector<rtc::scoped_refptr<rtc::RTCCertificate>>& certificates) {
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
   DCHECK(ice_transport_host);
   DCHECK(!ice_transport_host_);
   ice_transport_host_ = ice_transport_host;
  // TODO(https://crbug.com/874296): Pass through values for read and write
  // stream buffer sizes in the P2PQuicTransportConfig. Currently this is just
  // set to the same size as the QUIC receive window size (24 MB).
  uint32_t stream_buffer_size = 24 * 1024 * 1024;
   P2PQuicTransportConfig config(
       this, ice_transport_host->ConnectConsumer(this)->packet_transport(),
      certificates, stream_buffer_size);
   config.is_server = (perspective == quic::Perspective::IS_SERVER);
   quic_transport_ =
       quic_transport_factory_->CreateQuicTransport(std::move(config));
}
