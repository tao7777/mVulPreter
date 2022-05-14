 void GpuProcessHost::OnProcessLaunched() {
   UMA_HISTOGRAM_TIMES("GPU.GPUProcessLaunchTime",
                       base::TimeTicks::Now() - init_start_time_);
 }
