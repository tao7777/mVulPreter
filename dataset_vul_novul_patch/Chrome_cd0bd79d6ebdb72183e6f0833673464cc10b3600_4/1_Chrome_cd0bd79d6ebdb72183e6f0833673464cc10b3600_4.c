 GpuProcessHost::GpuProcessHost(int host_id, GpuProcessKind kind)
     : host_id_(host_id),
      gpu_process_(base::kNullProcessHandle),
       in_process_(false),
       software_rendering_(false),
       kind_(kind),
      process_launched_(false) {
  if (CommandLine::ForCurrentProcess()->HasSwitch(switches::kSingleProcess) ||
      CommandLine::ForCurrentProcess()->HasSwitch(switches::kInProcessGPU))
    in_process_ = true;

  DCHECK(!in_process_ || g_gpu_process_hosts[kind] == NULL);

  g_gpu_process_hosts[kind] = this;

  BrowserThread::PostTask(
      BrowserThread::UI,
      FROM_HERE,
      base::Bind(base::IgnoreResult(&GpuProcessHostUIShim::Create), host_id));

  process_.reset(
      new BrowserChildProcessHostImpl(content::PROCESS_TYPE_GPU, this));
}
