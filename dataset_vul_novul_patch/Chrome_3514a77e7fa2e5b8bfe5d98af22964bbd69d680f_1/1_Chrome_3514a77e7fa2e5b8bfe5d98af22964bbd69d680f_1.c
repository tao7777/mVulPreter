   bool ReceiverWasAdded(const RtpTransceiverState& transceiver_state) {
     uintptr_t receiver_id = RTCRtpReceiver::getId(
         transceiver_state.receiver_state()->webrtc_receiver().get());
     for (const auto& receiver : handler_->rtp_receivers_) {
      if (receiver->Id() == receiver_id)
        return false;
    }
    return true;
  }
