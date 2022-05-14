 void BrowserGpuChannelHostFactory::GpuChannelEstablishedOnIO(
     EstablishRequest* request,
     const IPC::ChannelHandle& channel_handle,
     const GPUInfo& gpu_info) {
   request->channel_handle = channel_handle;
   request->gpu_info = gpu_info;
   request->event.Signal();
 }
