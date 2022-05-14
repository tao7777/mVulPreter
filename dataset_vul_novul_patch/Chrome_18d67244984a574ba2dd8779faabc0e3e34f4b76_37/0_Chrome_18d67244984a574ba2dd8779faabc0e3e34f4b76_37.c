void RenderWidgetHostViewGtk::AcceleratedSurfaceBuffersSwapped(
     const GpuHostMsg_AcceleratedSurfaceBuffersSwapped_Params& params,
     int gpu_host_id) {
   RenderWidgetHostImpl::AcknowledgeBufferPresent(
      params.route_id, gpu_host_id, params.surface_handle, 0);
 }
