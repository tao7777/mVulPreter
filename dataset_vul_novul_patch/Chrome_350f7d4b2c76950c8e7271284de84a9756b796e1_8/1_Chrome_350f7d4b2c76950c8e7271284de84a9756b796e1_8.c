void QuicTransportHost::Initialize(
    IceTransportHost* ice_transport_host,
    quic::Perspective perspective,
    const std::vector<rtc::scoped_refptr<rtc::RTCCertificate>>& certificates) {
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
   DCHECK(ice_transport_host);
   DCHECK(!ice_transport_host_);
   ice_transport_host_ = ice_transport_host;
   P2PQuicTransportConfig config(
       this, ice_transport_host->ConnectConsumer(this)->packet_transport(),
      certificates);
   config.is_server = (perspective == quic::Perspective::IS_SERVER);
   quic_transport_ =
       quic_transport_factory_->CreateQuicTransport(std::move(config));
}
