 void VRDisplay::OnFocus() {
   display_blurred_ = false;
   ConnectVSyncProvider();
   navigator_vr_->EnqueueVREvent(VRDisplayEvent::Create(
      EventTypeNames::vrdisplayfocus, true, false, this, ""));
}
