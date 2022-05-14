bool GpuChannel::OnControlMessageReceived(const IPC::Message& msg) {
   bool handled = true;
   IPC_BEGIN_MESSAGE_MAP(GpuChannel, msg)
     IPC_MESSAGE_HANDLER_DELAY_REPLY(GpuChannelMsg_CreateOffscreenCommandBuffer,
                                     OnCreateOffscreenCommandBuffer)
     IPC_MESSAGE_HANDLER_DELAY_REPLY(GpuChannelMsg_DestroyCommandBuffer,
                                    OnDestroyCommandBuffer)
    IPC_MESSAGE_HANDLER_DELAY_REPLY(GpuChannelMsg_WillGpuSwitchOccur,
                                    OnWillGpuSwitchOccur)
    IPC_MESSAGE_HANDLER(GpuChannelMsg_CloseChannel, OnCloseChannel)
    IPC_MESSAGE_UNHANDLED(handled = false)
  IPC_END_MESSAGE_MAP()
  DCHECK(handled) << msg.type();
  return handled;
}
