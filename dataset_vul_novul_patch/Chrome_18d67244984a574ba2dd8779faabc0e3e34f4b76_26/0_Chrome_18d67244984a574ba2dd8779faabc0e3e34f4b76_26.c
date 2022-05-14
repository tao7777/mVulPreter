 void RenderWidgetHostViewAura::InsertSyncPointAndACK(
    const BufferPresentedParams& params) {
   uint32 sync_point = 0;
  // If we produced a texture, we have to synchronize with the consumer of
  // that texture.
  if (params.texture_to_produce) {
    params.texture_to_produce->Produce();
    sync_point = ImageTransportFactory::GetInstance()->InsertSyncPoint();
   }
 
   RenderWidgetHostImpl::AcknowledgeBufferPresent(
      params.route_id, params.gpu_host_id, params.surface_handle, sync_point);
 }
