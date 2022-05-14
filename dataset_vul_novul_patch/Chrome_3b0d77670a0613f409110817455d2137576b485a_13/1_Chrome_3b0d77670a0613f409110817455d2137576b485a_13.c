IPC::PlatformFileForTransit ProxyChannelDelegate::ShareHandleWithRemote(
    base::PlatformFile handle,
    const IPC::SyncChannel& channel,
    bool should_close_source) {
  return content::BrokerGetFileHandleForProcess(handle, channel.peer_pid(),
                                                should_close_source);
}
