  void SetupConnectedStreams() {
    CallbackRunLoop run_loop(runner());
    ASSERT_TRUE(client_peer_->quic_transport()->IsEncryptionEstablished());
    ASSERT_TRUE(server_peer_->quic_transport()->IsEncryptionEstablished());

    client_peer_->CreateStreamWithDelegate();
    ASSERT_TRUE(client_peer_->stream());
    ASSERT_TRUE(client_peer_->stream_delegate());

    base::RepeatingCallback<void()> callback = run_loop.CreateCallback();
    QuicPeerForTest* server_peer_ptr = server_peer_.get();
    MockP2PQuicStreamDelegate* stream_delegate =
        new MockP2PQuicStreamDelegate();
    P2PQuicStream* server_stream;
    EXPECT_CALL(*server_peer_->quic_transport_delegate(), OnStream(_))
        .WillOnce(Invoke([&callback, &server_stream,
                          &stream_delegate](P2PQuicStream* stream) {
          stream->SetDelegate(stream_delegate);
          server_stream = stream;
           callback.Run();
         }));
 
    client_peer_->stream()->WriteOrBufferData(kTriggerRemoteStreamPhrase,
                                              /*fin=*/false, nullptr);
     run_loop.RunUntilCallbacksFired();
    server_peer_ptr->SetStreamAndDelegate(
        static_cast<P2PQuicStreamImpl*>(server_stream),
        std::unique_ptr<MockP2PQuicStreamDelegate>(stream_delegate));
    ASSERT_TRUE(client_peer_->stream());
    ASSERT_TRUE(client_peer_->stream_delegate());
  }
