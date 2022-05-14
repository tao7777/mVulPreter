void CmdBufferImageTransportFactory::DestroySharedSurfaceHandle(
    const gfx::GLSurfaceHandle& handle) {
  if (!context_->makeContextCurrent()) {
     NOTREACHED() << "Failed to make shared graphics context current";
     return;
   }
 }
