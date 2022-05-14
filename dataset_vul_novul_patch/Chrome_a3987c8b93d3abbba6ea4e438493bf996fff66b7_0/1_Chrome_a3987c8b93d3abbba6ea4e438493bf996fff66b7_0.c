 OffscreenCanvasSurfaceImpl::OffscreenCanvasSurfaceImpl()
    : id_allocator_(new cc::SurfaceIdAllocator(AllocateSurfaceClientId())) {
  GetSurfaceManager()->RegisterSurfaceClientId(id_allocator_->client_id());
}
