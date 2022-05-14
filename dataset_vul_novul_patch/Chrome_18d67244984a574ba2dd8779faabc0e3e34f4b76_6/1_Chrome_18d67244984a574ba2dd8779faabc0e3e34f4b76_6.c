void GpuProcessHostUIShim::OnAcceleratedSurfacePostSubBuffer(
    const GpuHostMsg_AcceleratedSurfacePostSubBuffer_Params& params) {
  TRACE_EVENT0("renderer",
      "GpuProcessHostUIShim::OnAcceleratedSurfacePostSubBuffer");
 
   ScopedSendOnIOThread delayed_send(
       host_id_,
      new AcceleratedSurfaceMsg_BufferPresented(params.route_id, false, 0));
 
   RenderWidgetHostViewPort* view =
       GetRenderWidgetHostViewFromSurfaceID(params.surface_id);
  if (!view)
    return;

  delayed_send.Cancel();

  view->AcceleratedSurfacePostSubBuffer(params, host_id_);
}
