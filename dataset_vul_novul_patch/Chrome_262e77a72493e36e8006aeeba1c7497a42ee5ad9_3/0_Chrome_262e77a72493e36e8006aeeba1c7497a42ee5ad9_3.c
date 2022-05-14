 void VRDisplay::OnFocus() {
  DVLOG(1) << __FUNCTION__;
   display_blurred_ = false;
   ConnectVSyncProvider();
   navigator_vr_->EnqueueVREvent(VRDisplayEvent::Create(
      EventTypeNames::vrdisplayfocus, true, false, this, ""));
}
