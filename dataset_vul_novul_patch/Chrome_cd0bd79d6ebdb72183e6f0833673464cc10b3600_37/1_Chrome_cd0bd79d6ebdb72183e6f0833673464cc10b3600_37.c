GpuChannelHost* RenderThreadImpl::EstablishGpuChannelSync(
    content::CauseForGpuLaunch cause_for_gpu_launch) {
  if (gpu_channel_.get()) {
    if (gpu_channel_->state() == GpuChannelHost::kUnconnected ||
        gpu_channel_->state() == GpuChannelHost::kConnected)
      return GetGpuChannel();

    gpu_channel_ = NULL;
  }

   int client_id = 0;
   IPC::ChannelHandle channel_handle;
  base::ProcessHandle renderer_process_for_gpu;
   content::GPUInfo gpu_info;
   if (!Send(new GpuHostMsg_EstablishGpuChannel(cause_for_gpu_launch,
                                                &client_id,
                                                &channel_handle,
                                               &renderer_process_for_gpu,
                                                &gpu_info)) ||
      channel_handle.name.empty() ||
 #if defined(OS_POSIX)
       channel_handle.socket.fd == -1 ||
 #endif
      renderer_process_for_gpu == base::kNullProcessHandle) {
     gpu_channel_ = NULL;
     return NULL;
  }

  gpu_channel_ = new GpuChannelHost(this, 0, client_id);
  gpu_channel_->set_gpu_info(gpu_info);
   content::GetContentClient()->SetGpuInfo(gpu_info);
 
  gpu_channel_->Connect(channel_handle, renderer_process_for_gpu);
 
   return GetGpuChannel();
 }
