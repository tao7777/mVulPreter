 void GpuProcessHost::OnProcessLaunched() {
  base::ProcessHandle child_handle = in_process_ ?
      base::GetCurrentProcessHandle() : process_->GetData().handle;
#if defined(OS_WIN)
  DuplicateHandle(base::GetCurrentProcessHandle(),
                  child_handle,
                  base::GetCurrentProcessHandle(),
                  &gpu_process_,
                  PROCESS_DUP_HANDLE,
                  FALSE,
                  0);
#else
  gpu_process_ = child_handle;
#endif
   UMA_HISTOGRAM_TIMES("GPU.GPUProcessLaunchTime",
                       base::TimeTicks::Now() - init_start_time_);
 }
