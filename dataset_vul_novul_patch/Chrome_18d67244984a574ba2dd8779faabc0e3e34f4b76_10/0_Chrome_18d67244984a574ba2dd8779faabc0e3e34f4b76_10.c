CmdBufferImageTransportFactory::CreateSharedSurfaceHandle() {
  if (!context_->makeContextCurrent()) {
    NOTREACHED() << "Failed to make shared graphics context current";
    return gfx::GLSurfaceHandle();
  }

   gfx::GLSurfaceHandle handle = gfx::GLSurfaceHandle(
       gfx::kNullPluginWindow, true);
   handle.parent_gpu_process_id = context_->GetGPUProcessID();
   context_->flush();
   return handle;
 }
