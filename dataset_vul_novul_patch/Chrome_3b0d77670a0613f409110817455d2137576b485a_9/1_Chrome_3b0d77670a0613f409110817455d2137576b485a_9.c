base::WaitableEvent* ProxyChannelDelegate::GetShutdownEvent() {
  return &shutdown_event_;
}
