void GpuProcessHost::OnProcessCrashed(int exit_code) {
   int process_crash_exit_code = exit_code;
   base::debug::Alias(&process_crash_exit_code);
 
  // Record crash before doing anything that could start a new GPU process.
  RecordProcessCrash();

  if (activity_flags_.IsFlagSet(
          gpu::ActivityFlagsBase::FLAG_LOADING_PROGRAM_BINARY)) {
    for (auto cache_key : client_id_to_shader_cache_) {
      GetShaderCacheFactorySingleton()->ClearByClientId(
          cache_key.first, base::Time(), base::Time::Max(), base::Bind([] {}));
     }
   }
   SendOutstandingReplies(EstablishChannelStatus::GPU_HOST_INVALID);
 
   ChildProcessTerminationInfo info =
       process_->GetTerminationInfo(true /* known_dead */);
  GpuDataManagerImpl::GetInstance()->ProcessCrashed(info.status);
}
