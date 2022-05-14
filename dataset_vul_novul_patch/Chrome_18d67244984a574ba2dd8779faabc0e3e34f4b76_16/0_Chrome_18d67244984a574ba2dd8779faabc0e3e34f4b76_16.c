 void RenderWidgetHostViewAndroid::AcceleratedSurfaceBuffersSwapped(
     const GpuHostMsg_AcceleratedSurfaceBuffersSwapped_Params& params,
     int gpu_host_id) {
  ImageTransportFactoryAndroid* factory =
      ImageTransportFactoryAndroid::GetInstance();
 
  // need to delay the ACK until after commit and use more than a single
  // texture.
   DCHECK(!CompositorImpl::IsThreadingEnabled());

  uint64 previous_buffer = current_buffer_id_;
  if (previous_buffer && texture_id_in_layer_) {
    DCHECK(id_to_mailbox_.find(previous_buffer) != id_to_mailbox_.end());
    ImageTransportFactoryAndroid::GetInstance()->ReleaseTexture(
        texture_id_in_layer_,
        reinterpret_cast<const signed char*>(
            id_to_mailbox_[previous_buffer].c_str()));
  }

  current_buffer_id_ = params.surface_handle;
  if (!texture_id_in_layer_) {
    texture_id_in_layer_ = factory->CreateTexture();
    texture_layer_->setTextureId(texture_id_in_layer_);
  }

  DCHECK(id_to_mailbox_.find(current_buffer_id_) != id_to_mailbox_.end());
  ImageTransportFactoryAndroid::GetInstance()->AcquireTexture(
      texture_id_in_layer_,
      reinterpret_cast<const signed char*>(
          id_to_mailbox_[current_buffer_id_].c_str()));
  texture_layer_->setNeedsDisplay();
  texture_layer_->setBounds(params.size);
  texture_size_in_layer_ = params.size;

   uint32 sync_point =
       ImageTransportFactoryAndroid::GetInstance()->InsertSyncPoint();
   RenderWidgetHostImpl::AcknowledgeBufferPresent(
      params.route_id, gpu_host_id, previous_buffer, sync_point);
 }
