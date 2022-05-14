void PluginChannel::OnChannelConnected(int32 peer_pid) {
  base::ProcessHandle handle;
  if (!base::OpenProcessHandle(peer_pid, &handle)) {
    NOTREACHED();
  }
  renderer_handle_ = handle;
  NPChannelBase::OnChannelConnected(peer_pid);
}
