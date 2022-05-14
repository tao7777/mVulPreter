void GpuProcessHostUIShim::OnAcceleratedSurfaceRelease(
    const GpuHostMsg_AcceleratedSurfaceRelease_Params& params) {
  RenderWidgetHostViewPort* view = GetRenderWidgetHostViewFromSurfaceID(
       params.surface_id);
   if (!view)
     return;
  view->AcceleratedSurfaceRelease(params.identifier);
 }
