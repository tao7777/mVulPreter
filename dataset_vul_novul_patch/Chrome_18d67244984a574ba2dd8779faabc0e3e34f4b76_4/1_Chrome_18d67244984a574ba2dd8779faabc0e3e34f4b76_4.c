void GpuProcessHostUIShim::OnAcceleratedSurfaceBuffersSwapped(
    const GpuHostMsg_AcceleratedSurfaceBuffersSwapped_Params& params) {
  TRACE_EVENT0("renderer",
      "GpuProcessHostUIShim::OnAcceleratedSurfaceBuffersSwapped");
 
   ScopedSendOnIOThread delayed_send(
       host_id_,
      new AcceleratedSurfaceMsg_BufferPresented(params.route_id, false, 0));
 
   RenderWidgetHostViewPort* view = GetRenderWidgetHostViewFromSurfaceID(
       params.surface_id);
  if (!view)
    return;

  delayed_send.Cancel();

  static const base::TimeDelta swap_delay = GetSwapDelay();
  if (swap_delay.ToInternalValue())
    base::PlatformThread::Sleep(swap_delay);

  view->AcceleratedSurfaceBuffersSwapped(params, host_id_);
}
