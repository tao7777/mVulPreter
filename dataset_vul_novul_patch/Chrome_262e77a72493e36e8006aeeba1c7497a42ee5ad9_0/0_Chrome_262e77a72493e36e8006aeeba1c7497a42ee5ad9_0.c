void VRDisplay::ConnectVSyncProvider() {
  DVLOG(1) << __FUNCTION__
           << ": IsPresentationFocused()=" << IsPresentationFocused()
           << " vr_v_sync_provider_.is_bound()="
           << vr_v_sync_provider_.is_bound();
  if (!IsPresentationFocused() || vr_v_sync_provider_.is_bound())
    return;
   display_->GetVRVSyncProvider(mojo::MakeRequest(&vr_v_sync_provider_));
   vr_v_sync_provider_.set_connection_error_handler(ConvertToBaseCallback(
       WTF::Bind(&VRDisplay::OnVSyncConnectionError, WrapWeakPersistent(this))));
  if (pending_vrdisplay_raf_ && !display_blurred_) {
     pending_vsync_ = true;
     vr_v_sync_provider_->GetVSync(ConvertToBaseCallback(
         WTF::Bind(&VRDisplay::OnVSync, WrapWeakPersistent(this))));
  }
}
