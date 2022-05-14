   void ProcessStateChangesPlanB(WebRtcSetDescriptionObserver::States states) {
     DCHECK_EQ(sdp_semantics_, webrtc::SdpSemantics::kPlanB);
    if (!handler_)
      return;

     std::vector<RTCRtpReceiver*> removed_receivers;
    for (auto it = handler_->rtp_receivers_.begin();
         it != handler_->rtp_receivers_.end(); ++it) {
      if (ReceiverWasRemoved(*(*it), states.transceiver_states))
        removed_receivers.push_back(it->get());
    }
 
     for (auto& transceiver_state : states.transceiver_states) {
      if (handler_ && ReceiverWasAdded(transceiver_state)) {
        // |handler_| can become null after this call.
         handler_->OnAddReceiverPlanB(transceiver_state.MoveReceiverState());
       }
     }
     for (auto* removed_receiver : removed_receivers) {
      if (handler_) {
        // |handler_| can become null after this call.
        handler_->OnRemoveReceiverPlanB(RTCRtpReceiver::getId(
            removed_receiver->state().webrtc_receiver().get()));
      }
     }
   }
