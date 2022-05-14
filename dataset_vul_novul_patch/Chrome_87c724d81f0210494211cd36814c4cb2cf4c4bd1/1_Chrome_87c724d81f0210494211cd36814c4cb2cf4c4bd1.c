void GpuDataManager::UpdateGpuFeatureFlags() {
  if (!BrowserThread::CurrentlyOn(BrowserThread::UI)) {
    BrowserThread::PostTask(BrowserThread::UI, FROM_HERE,
        NewRunnableMethod(this, &GpuDataManager::UpdateGpuFeatureFlags));
    return;
   }
 
   GpuBlacklist* gpu_blacklist = GetGpuBlacklist();
  if (gpu_blacklist == NULL)
    return;
   if (!gpu_blacklist) {
    gpu_feature_flags_.set_flags(0);
    return;
  }

  {
    base::AutoLock auto_lock(gpu_info_lock_);
    gpu_feature_flags_ = gpu_blacklist->DetermineGpuFeatureFlags(
        GpuBlacklist::kOsAny, NULL, gpu_info_);
  }

  uint32 max_entry_id = gpu_blacklist->max_entry_id();
  if (!gpu_feature_flags_.flags()) {
    UMA_HISTOGRAM_ENUMERATION("GPU.BlacklistTestResultsPerEntry",
        0, max_entry_id + 1);
    return;
  }

  RunGpuInfoUpdateCallbacks();

  std::vector<uint32> flag_entries;
  gpu_blacklist->GetGpuFeatureFlagEntries(
      GpuFeatureFlags::kGpuFeatureAll, flag_entries);
  DCHECK_GT(flag_entries.size(), 0u);
  for (size_t i = 0; i < flag_entries.size(); ++i) {
    UMA_HISTOGRAM_ENUMERATION("GPU.BlacklistTestResultsPerEntry",
        flag_entries[i], max_entry_id + 1);
  }
}
