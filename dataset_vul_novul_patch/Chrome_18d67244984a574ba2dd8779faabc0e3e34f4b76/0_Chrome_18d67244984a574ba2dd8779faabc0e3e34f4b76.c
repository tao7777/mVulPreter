void BrowserPluginGuest::SetCompositingBufferData(int gpu_process_id,
                                                  uint32 client_id,
                                                  uint32 context_id,
                                                  uint32 texture_id_0,
                                                  uint32 texture_id_1,
                                                  uint32 sync_point) {
  if (texture_id_0 == 0) {
    DCHECK(texture_id_1 == 0);
    return;
  }

  DCHECK(texture_id_1 != 0);
  DCHECK(texture_id_0 != texture_id_1);

   surface_handle_ = gfx::GLSurfaceHandle(gfx::kNullPluginWindow, true);
   surface_handle_.parent_gpu_process_id = gpu_process_id;
   surface_handle_.parent_client_id = client_id;
 }
