 void VRDisplay::OnPresentChange() {
   if (is_presenting_ && !is_valid_device_for_presenting_) {
     DVLOG(1) << __FUNCTION__ << ": device not valid, not sending event";
     return;
  }
  navigator_vr_->EnqueueVREvent(VRDisplayEvent::Create(
      EventTypeNames::vrdisplaypresentchange, true, false, this, ""));
}
