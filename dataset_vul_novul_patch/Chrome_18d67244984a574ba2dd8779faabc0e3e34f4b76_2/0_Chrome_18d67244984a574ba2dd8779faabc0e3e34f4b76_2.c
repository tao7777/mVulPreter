void GpuProcessHost::OnAcceleratedSurfaceBuffersSwapped(
    const GpuHostMsg_AcceleratedSurfaceBuffersSwapped_Params& params) {
  TRACE_EVENT0("gpu", "GpuProcessHost::OnAcceleratedSurfaceBuffersSwapped");

  gfx::PluginWindowHandle handle =
      GpuSurfaceTracker::Get()->GetSurfaceWindowHandle(params.surface_id);
  if (handle != gfx::kNullPluginWindow) {
    RouteOnUIThread(GpuHostMsg_AcceleratedSurfaceBuffersSwapped(params));
    return;
  }

   base::ScopedClosureRunner scoped_completion_runner(
       base::Bind(&AcceleratedSurfaceBuffersSwappedCompletedForGPU,
                  host_id_, params.route_id,
                 true /* alive */, params.surface_handle));
 
   int render_process_id = 0;
   int render_widget_id = 0;
  if (!GpuSurfaceTracker::Get()->GetRenderWidgetIDForSurface(
      params.surface_id, &render_process_id, &render_widget_id)) {
    return;
  }
  RenderWidgetHelper* helper =
      RenderWidgetHelper::FromProcessHostID(render_process_id);
  if (!helper)
    return;

  scoped_completion_runner.Release();
  helper->DidReceiveBackingStoreMsg(ViewHostMsg_CompositorSurfaceBuffersSwapped(
      render_widget_id,
      params.surface_id,
      params.surface_handle,
      params.route_id,
      params.size,
      host_id_));
}
