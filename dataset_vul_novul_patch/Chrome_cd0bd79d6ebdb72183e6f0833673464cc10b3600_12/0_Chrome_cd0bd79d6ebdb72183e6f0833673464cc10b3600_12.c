 GpuChannelHost::GpuChannelHost(
    GpuChannelHostFactory* factory, int gpu_host_id, int client_id)
     : factory_(factory),
       client_id_(client_id),
      gpu_host_id_(gpu_host_id),
       state_(kUnconnected) {
 }
