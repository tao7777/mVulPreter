 void RenderWidgetHostViewAndroid::AcceleratedSurfaceBuffersSwapped(
     const GpuHostMsg_AcceleratedSurfaceBuffersSwapped_Params& params,
     int gpu_host_id) {
  texture_layer_->setTextureId(params.surface_handle);
  DCHECK(texture_layer_ == layer_);
  layer_->setBounds(params.size);
  texture_id_in_layer_ = params.surface_handle;
  texture_size_in_layer_ = params.size;
 
   DCHECK(!CompositorImpl::IsThreadingEnabled());
   uint32 sync_point =
       ImageTransportFactoryAndroid::GetInstance()->InsertSyncPoint();
   RenderWidgetHostImpl::AcknowledgeBufferPresent(
      params.route_id, gpu_host_id, true, sync_point);
 }
