 void VRDisplay::OnBlur() {
   display_blurred_ = true;
   vr_v_sync_provider_.reset();
   navigator_vr_->EnqueueVREvent(VRDisplayEvent::Create(
       EventTypeNames::vrdisplayblur, true, false, this, ""));
 }
