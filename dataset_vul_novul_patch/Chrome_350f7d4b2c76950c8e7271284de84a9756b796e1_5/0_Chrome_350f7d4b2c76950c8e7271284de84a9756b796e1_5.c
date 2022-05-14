  void Initialize(bool can_respond_to_crypto_handshake = true) {
    clock_.AdvanceTime(quic::QuicTime::Delta::FromMilliseconds(1000));
    runner_ = new net::test::TestTaskRunner(&clock_);
    net::QuicChromiumAlarmFactory* alarm_factory =
        new net::QuicChromiumAlarmFactory(runner_.get(), &clock_);
    quic_transport_factory_ = std::make_unique<P2PQuicTransportFactoryImpl>(
        &clock_, std::unique_ptr<net::QuicChromiumAlarmFactory>(alarm_factory));

    auto client_packet_transport =
        std::make_unique<FakePacketTransport>(alarm_factory, &clock_);
    auto server_packet_transport =
        std::make_unique<FakePacketTransport>(alarm_factory, &clock_);
    client_packet_transport->ConnectPeerTransport(
        server_packet_transport.get());
    server_packet_transport->ConnectPeerTransport(
        client_packet_transport.get());
    rtc::scoped_refptr<rtc::RTCCertificate> client_cert =
        CreateTestCertificate();

    auto client_quic_transport_delegate =
        std::make_unique<MockP2PQuicTransportDelegate>();
    std::vector<rtc::scoped_refptr<rtc::RTCCertificate>> client_certificates;
     client_certificates.push_back(client_cert);
     P2PQuicTransportConfig client_config(client_quic_transport_delegate.get(),
                                          client_packet_transport.get(),
                                         client_certificates, kWriteBufferSize);
     client_config.is_server = false;
     client_config.can_respond_to_crypto_handshake =
         can_respond_to_crypto_handshake;
    P2PQuicTransportImpl* client_quic_transport_ptr =
        static_cast<P2PQuicTransportImpl*>(
            quic_transport_factory_
                ->CreateQuicTransport(std::move(client_config))
                .release());
    std::unique_ptr<P2PQuicTransportImpl> client_quic_transport =
        std::unique_ptr<P2PQuicTransportImpl>(client_quic_transport_ptr);
    client_peer_ = std::make_unique<QuicPeerForTest>(
        std::move(client_packet_transport),
        std::move(client_quic_transport_delegate),
        std::move(client_quic_transport), client_cert);

    auto server_quic_transport_delegate =
        std::make_unique<MockP2PQuicTransportDelegate>();

    rtc::scoped_refptr<rtc::RTCCertificate> server_cert =
        CreateTestCertificate();
    std::vector<rtc::scoped_refptr<rtc::RTCCertificate>> server_certificates;
     server_certificates.push_back(server_cert);
     P2PQuicTransportConfig server_config(server_quic_transport_delegate.get(),
                                          server_packet_transport.get(),
                                         server_certificates, kWriteBufferSize);
     server_config.is_server = true;
     server_config.can_respond_to_crypto_handshake =
         can_respond_to_crypto_handshake;
    P2PQuicTransportImpl* server_quic_transport_ptr =
        static_cast<P2PQuicTransportImpl*>(
            quic_transport_factory_
                ->CreateQuicTransport(std::move(server_config))
                .release());
    std::unique_ptr<P2PQuicTransportImpl> server_quic_transport =
        std::unique_ptr<P2PQuicTransportImpl>(server_quic_transport_ptr);
    server_peer_ = std::make_unique<QuicPeerForTest>(
        std::move(server_packet_transport),
        std::move(server_quic_transport_delegate),
        std::move(server_quic_transport), server_cert);
  }
