 void PluginChannel::OnChannelError() {
   NPChannelBase::OnChannelError();
   CleanUp();
 }
