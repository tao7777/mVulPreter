void GpuChannel::OnChannelConnected(int32 peer_pid) {
  renderer_pid_ = peer_pid;
}
