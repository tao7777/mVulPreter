 bool NaClProcessHost::SendStart() {
  if (!enable_ipc_proxy_) {
    if (!ReplyToRenderer(IPC::ChannelHandle()))
      return false;
  }
  return StartNaClExecution();
}
