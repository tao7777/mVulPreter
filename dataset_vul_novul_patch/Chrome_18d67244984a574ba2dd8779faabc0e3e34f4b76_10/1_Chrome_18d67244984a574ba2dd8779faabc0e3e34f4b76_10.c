CmdBufferImageTransportFactory::CreateSharedSurfaceHandle() {
  if (!context_->makeContextCurrent()) {
    NOTREACHED() << "Failed to make shared graphics context current";
    return gfx::GLSurfaceHandle();
  }

   gfx::GLSurfaceHandle handle = gfx::GLSurfaceHandle(
       gfx::kNullPluginWindow, true);
   handle.parent_gpu_process_id = context_->GetGPUProcessID();
  handle.parent_client_id = context_->GetChannelID();
  handle.parent_context_id = context_->GetContextID();
  handle.parent_texture_id[0] = context_->createTexture();
  handle.parent_texture_id[1] = context_->createTexture();
  handle.sync_point = context_->insertSyncPoint();
   context_->flush();
   return handle;
 }
