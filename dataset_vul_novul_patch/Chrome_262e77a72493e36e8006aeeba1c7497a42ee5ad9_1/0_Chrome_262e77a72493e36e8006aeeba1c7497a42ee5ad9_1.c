 void VRDisplay::FocusChanged() {
  DVLOG(1) << __FUNCTION__;
   vr_v_sync_provider_.reset();
   ConnectVSyncProvider();
 }
