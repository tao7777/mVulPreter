void BaseAudioContext::Initialize() {
  if (IsDestinationInitialized())
    return;

  FFTFrame::Initialize();

  audio_worklet_ = AudioWorklet::Create(this);
 
   if (destination_node_) {
     destination_node_->Handler().Initialize();
    // TODO(crbug.com/863951).  The audio thread needs some things from the
    // destination handler like the currentTime.  But the audio thread
    // shouldn't access the |destination_node_| since it's an Oilpan object.
    // Thus, get the destination handler, a non-oilpan object, so we can get
    // the items directly from the handler instead of through the destination
    // node.
    destination_handler_ = &destination_node_->GetAudioDestinationHandler();

     listener_ = AudioListener::Create(*this);
  }
}
