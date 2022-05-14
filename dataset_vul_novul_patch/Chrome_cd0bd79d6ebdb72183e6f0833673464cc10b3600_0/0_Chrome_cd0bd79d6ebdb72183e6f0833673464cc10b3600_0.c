GpuChannelHost* BrowserGpuChannelHostFactory::EstablishGpuChannelSync(
    CauseForGpuLaunch cause_for_gpu_launch) {
  if (gpu_channel_.get()) {
    if (gpu_channel_->state() == GpuChannelHost::kLost)
      gpu_channel_ = NULL;
    else
      return gpu_channel_.get();
  }
  GpuDataManagerImpl::GetInstance();

  EstablishRequest request;
  GetIOLoopProxy()->PostTask(
      FROM_HERE,
      base::Bind(
          &BrowserGpuChannelHostFactory::EstablishGpuChannelOnIO,
          base::Unretained(this),
          &request,
          cause_for_gpu_launch));
   request.event.Wait();
 
  if (request.channel_handle.name.empty())
     return NULL;
 
   gpu_channel_ = new GpuChannelHost(this, gpu_host_id_, gpu_client_id_);
   gpu_channel_->set_gpu_info(request.gpu_info);
   content::GetContentClient()->SetGpuInfo(request.gpu_info);
 
  gpu_channel_->Connect(request.channel_handle);
 
   return gpu_channel_.get();
 }
