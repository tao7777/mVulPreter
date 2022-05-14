 void VRDisplay::FocusChanged() {
   vr_v_sync_provider_.reset();
   ConnectVSyncProvider();
 }
