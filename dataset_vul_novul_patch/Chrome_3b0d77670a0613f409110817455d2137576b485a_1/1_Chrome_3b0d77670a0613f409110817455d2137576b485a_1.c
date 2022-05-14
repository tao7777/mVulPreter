void NaClProcessHost::OnPpapiChannelCreated(
    const IPC::ChannelHandle& channel_handle) {
  DCHECK(enable_ipc_proxy_);
  ReplyToRenderer(channel_handle);
 }
