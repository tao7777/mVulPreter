 void RenderWidgetHostViewGtk::AcceleratedSurfacePostSubBuffer(
     const GpuHostMsg_AcceleratedSurfacePostSubBuffer_Params& params,
     int gpu_host_id) {
   RenderWidgetHostImpl::AcknowledgeBufferPresent(
      params.route_id, gpu_host_id, true, 0);
 }
