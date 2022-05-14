 void RenderWidgetHostViewAura::InsertSyncPointAndACK(
    int32 route_id, int gpu_host_id, bool presented,
    ui::Compositor* compositor) {
   uint32 sync_point = 0;
  if (compositor) {
    ImageTransportFactory* factory = ImageTransportFactory::GetInstance();
    sync_point = factory->InsertSyncPoint();
   }
 
   RenderWidgetHostImpl::AcknowledgeBufferPresent(
      route_id, gpu_host_id, presented, sync_point);
 }
