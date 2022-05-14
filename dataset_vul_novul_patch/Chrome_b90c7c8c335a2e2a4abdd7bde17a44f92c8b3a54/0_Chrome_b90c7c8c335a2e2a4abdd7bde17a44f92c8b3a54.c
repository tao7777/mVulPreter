void GpuProcessHost::DidFailInitialize() {
   UMA_HISTOGRAM_BOOLEAN("GPU.GPUProcessInitialized", false);
   status_ = FAILURE;
   GpuDataManagerImpl* gpu_data_manager = GpuDataManagerImpl::GetInstance();
  if (kind_ == GPU_PROCESS_KIND_SANDBOXED)
    gpu_data_manager->FallBackToNextGpuMode();
   RunRequestGPUInfoCallbacks(gpu_data_manager->GetGPUInfo());
 }
