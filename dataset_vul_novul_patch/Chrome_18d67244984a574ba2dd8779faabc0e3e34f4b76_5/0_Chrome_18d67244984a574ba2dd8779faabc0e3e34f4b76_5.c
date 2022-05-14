void GpuProcessHostUIShim::OnAcceleratedSurfaceNew(
    const GpuHostMsg_AcceleratedSurfaceNew_Params& params) {
  RenderWidgetHostViewPort* view = GetRenderWidgetHostViewFromSurfaceID(
       params.surface_id);
   if (!view)
     return;

  if (params.mailbox_name.length() &&
      params.mailbox_name.length() != GL_MAILBOX_SIZE_CHROMIUM)
    return;

   view->AcceleratedSurfaceNew(
      params.width, params.height, params.surface_handle,
      params.mailbox_name);
 }
