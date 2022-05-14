   QuicPacket* ConstructDataPacket(QuicPacketSequenceNumber number,
                                   QuicFecGroupNumber fec_group) {
    header_.guid = guid_;
     header_.packet_sequence_number = number;
    header_.transmission_time = 0;
    header_.retransmission_count = 0;
     header_.flags = PACKET_FLAGS_NONE;
     header_.fec_group = fec_group;
 
    QuicFrames frames;
    QuicFrame frame(&frame1_);
    frames.push_back(frame);
    QuicPacket* packet;
    framer_.ConstructFragementDataPacket(header_, frames, &packet);
    return packet;
  }
