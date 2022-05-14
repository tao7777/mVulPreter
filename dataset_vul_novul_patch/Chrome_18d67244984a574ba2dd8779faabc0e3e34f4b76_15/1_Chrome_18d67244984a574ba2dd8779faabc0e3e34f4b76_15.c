void RenderWidgetHostImpl::SendFrontSurfaceIsProtected(
    bool is_protected,
    uint32 protection_state_id,
    int32 route_id,
    int gpu_host_id) {
  GpuProcessHostUIShim* ui_shim = GpuProcessHostUIShim::FromID(gpu_host_id);
  if (ui_shim) {
    ui_shim->Send(new AcceleratedSurfaceMsg_SetFrontSurfaceIsProtected(
        route_id, is_protected, protection_state_id));
  }
}
