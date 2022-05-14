 int VRDisplay::requestAnimationFrame(FrameRequestCallback* callback) {
  DVLOG(2) << __FUNCTION__;
   Document* doc = this->GetDocument();
   if (!doc)
     return 0;
  pending_vrdisplay_raf_ = true;
   if (!vr_v_sync_provider_.is_bound()) {
     ConnectVSyncProvider();
   } else if (!display_blurred_ && !pending_vsync_) {
    pending_vsync_ = true;
    vr_v_sync_provider_->GetVSync(ConvertToBaseCallback(
        WTF::Bind(&VRDisplay::OnVSync, WrapWeakPersistent(this))));
  }
  callback->use_legacy_time_base_ = false;
  return EnsureScriptedAnimationController(doc).RegisterCallback(callback);
 }
