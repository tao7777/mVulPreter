 void GpuChannelHost::Connect(
    const IPC::ChannelHandle& channel_handle,
    base::ProcessHandle client_process_for_gpu) {
   DCHECK(factory_->IsMainThread());
  scoped_refptr<base::MessageLoopProxy> io_loop = factory_->GetIOLoopProxy();
  channel_.reset(new IPC::SyncChannel(
      channel_handle, IPC::Channel::MODE_CLIENT, NULL,
      io_loop, true,
      factory_->GetShutDownEvent()));

  sync_filter_ = new IPC::SyncMessageFilter(
      factory_->GetShutDownEvent());

  channel_->AddFilter(sync_filter_.get());

  channel_filter_ = new MessageFilter(this);

  channel_->AddFilter(channel_filter_.get());

   state_ = kConnected;
  Send(new GpuChannelMsg_Initialize(client_process_for_gpu));
 }
