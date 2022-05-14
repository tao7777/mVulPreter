void CmdBufferImageTransportFactory::DestroySharedSurfaceHandle(
    const gfx::GLSurfaceHandle& handle) {
  if (!context_->makeContextCurrent()) {
     NOTREACHED() << "Failed to make shared graphics context current";
     return;
   }
  context_->deleteTexture(handle.parent_texture_id[0]);
  context_->deleteTexture(handle.parent_texture_id[1]);
  context_->finish();
 }
