base::MessageLoopProxy* ProxyChannelDelegate::GetIPCMessageLoop() {
  return RenderThread::Get()->GetIOMessageLoopProxy().get();
}
