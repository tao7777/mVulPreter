 void VRDisplay::ProcessScheduledAnimations(double timestamp) {
   Document* doc = this->GetDocument();
  if (!doc || display_blurred_ || !scripted_animation_controller_)
     return;
 
   TRACE_EVENT1("gpu", "VRDisplay::OnVSync", "frame", vr_frame_id_);
 
  AutoReset<bool> animating(&in_animation_frame_, true);
  pending_raf_ = false;
  scripted_animation_controller_->ServiceScriptedAnimations(timestamp);
 
  if (is_presenting_ && !capabilities_->hasExternalDisplay()) {
    Platform::Current()->CurrentThread()->GetWebTaskRunner()->PostTask(
        BLINK_FROM_HERE, WTF::Bind(&VRDisplay::ProcessScheduledWindowAnimations,
                                   WrapWeakPersistent(this), timestamp));
  }
}
