 void VRDisplay::OnPresentChange() {
  DVLOG(1) << __FUNCTION__ << ": is_presenting_=" << is_presenting_;
   if (is_presenting_ && !is_valid_device_for_presenting_) {
     DVLOG(1) << __FUNCTION__ << ": device not valid, not sending event";
     return;
  }
  navigator_vr_->EnqueueVREvent(VRDisplayEvent::Create(
      EventTypeNames::vrdisplaypresentchange, true, false, this, ""));
}
