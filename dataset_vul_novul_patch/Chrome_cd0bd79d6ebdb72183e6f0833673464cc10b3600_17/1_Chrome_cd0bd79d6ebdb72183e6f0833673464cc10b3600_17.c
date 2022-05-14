void GpuChannel::OnInitialize(base::ProcessHandle renderer_process) {
  DCHECK(!renderer_process_);
  if (base::GetProcId(renderer_process) == renderer_pid_)
    renderer_process_ = renderer_process;
}
