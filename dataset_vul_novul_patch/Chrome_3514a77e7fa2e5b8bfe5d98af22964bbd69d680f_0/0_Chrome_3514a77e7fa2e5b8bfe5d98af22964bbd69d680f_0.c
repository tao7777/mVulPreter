   void ProcessStateChangesUnifiedPlan(
       WebRtcSetDescriptionObserver::States states) {
     DCHECK_EQ(sdp_semantics_, webrtc::SdpSemantics::kUnifiedPlan);
    if (handler_) {
      handler_->OnModifyTransceivers(
          std::move(states.transceiver_states),
          action_ == PeerConnectionTracker::ACTION_SET_REMOTE_DESCRIPTION);
    }
   }
