 void RenderWidgetHostImpl::AcknowledgeBufferPresent(
    int32 route_id, int gpu_host_id, bool presented, uint32 sync_point) {
   GpuProcessHostUIShim* ui_shim = GpuProcessHostUIShim::FromID(gpu_host_id);
   if (ui_shim)
     ui_shim->Send(new AcceleratedSurfaceMsg_BufferPresented(route_id,
                                                            presented,
                                                             sync_point));
 }
