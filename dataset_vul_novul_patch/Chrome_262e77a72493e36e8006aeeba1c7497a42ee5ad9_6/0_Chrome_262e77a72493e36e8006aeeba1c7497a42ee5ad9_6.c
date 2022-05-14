 void VRDisplay::ProcessScheduledAnimations(double timestamp) {
  DVLOG(2) << __FUNCTION__;
   Document* doc = this->GetDocument();
  if (!doc || display_blurred_) {
    DVLOG(2) << __FUNCTION__ << ": early exit, doc=" << doc
             << " display_blurred_=" << display_blurred_;
     return;
  }
 
   TRACE_EVENT1("gpu", "VRDisplay::OnVSync", "frame", vr_frame_id_);
 
  if (pending_vrdisplay_raf_ && scripted_animation_controller_) {
    // Run the callback, making sure that in_animation_frame_ is only
    // true for the vrDisplay rAF and not for a legacy window rAF
    // that may be called later.
    AutoReset<bool> animating(&in_animation_frame_, true);
    pending_vrdisplay_raf_ = false;
    scripted_animation_controller_->ServiceScriptedAnimations(timestamp);
  }
 
  if (is_presenting_ && !capabilities_->hasExternalDisplay()) {
    Platform::Current()->CurrentThread()->GetWebTaskRunner()->PostTask(
        BLINK_FROM_HERE, WTF::Bind(&VRDisplay::ProcessScheduledWindowAnimations,
                                   WrapWeakPersistent(this), timestamp));
  }
}
