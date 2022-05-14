void RenderWidgetHostImpl::OnCompositorSurfaceBuffersSwapped(
      int32 surface_id,
      uint64 surface_handle,
      int32 route_id,
      const gfx::Size& size,
      int32 gpu_process_host_id) {
  TRACE_EVENT0("renderer_host",
               "RenderWidgetHostImpl::OnCompositorSurfaceBuffersSwapped");
   if (!view_) {
     RenderWidgetHostImpl::AcknowledgeBufferPresent(route_id,
                                                    gpu_process_host_id,
                                                   surface_handle,
                                                    0);
     return;
   }
  GpuHostMsg_AcceleratedSurfaceBuffersSwapped_Params gpu_params;
  gpu_params.surface_id = surface_id;
  gpu_params.surface_handle = surface_handle;
  gpu_params.route_id = route_id;
  gpu_params.size = size;
#if defined(OS_MACOSX)
  gpu_params.window = gfx::kNullPluginWindow;
#endif
  view_->AcceleratedSurfaceBuffersSwapped(gpu_params,
                                          gpu_process_host_id);
}
