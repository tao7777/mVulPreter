 void PluginChannel::OnChannelError() {
  base::CloseProcessHandle(renderer_handle_);
  renderer_handle_ = 0;
   NPChannelBase::OnChannelError();
   CleanUp();
 }
