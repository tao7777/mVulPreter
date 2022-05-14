 P2PQuicStreamImpl::P2PQuicStreamImpl(quic::QuicStreamId id,
                                     quic::QuicSession* session)
    : quic::QuicStream(id, session, /*is_static=*/false, quic::BIDIRECTIONAL) {}
