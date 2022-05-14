 GpuChannelHost::GpuChannelHost(
    GpuChannelHostFactory* factory, int gpu_process_id, int client_id)
     : factory_(factory),
      gpu_process_id_(gpu_process_id),
       client_id_(client_id),
       state_(kUnconnected) {
 }
