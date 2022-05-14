void RenderProcessHostImpl::OnCompositorSurfaceBuffersSwappedNoHost(
      int32 surface_id,
      uint64 surface_handle,
      int32 route_id,
      const gfx::Size& size,
      int32 gpu_process_host_id) {
  TRACE_EVENT0("renderer_host",
                "RenderWidgetHostImpl::OnCompositorSurfaceBuffersSwappedNoHost");
   RenderWidgetHostImpl::AcknowledgeBufferPresent(route_id,
                                                  gpu_process_host_id,
                                                 surface_handle,
                                                  0);
 }
