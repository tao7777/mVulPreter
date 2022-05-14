void GpuDataManager::UpdateGpuInfo(const GPUInfo& gpu_info) {
  {
     base::AutoLock auto_lock(gpu_info_lock_);
     if (!gpu_info_.Merge(gpu_info))
       return;
  }

  RunGpuInfoUpdateCallbacks();
 
  {
    base::AutoLock auto_lock(gpu_info_lock_);
     content::GetContentClient()->SetGpuInfo(gpu_info_);
   }
 
  UpdateGpuFeatureFlags();
}
