 void HostNPScriptObject::OnShutdown() {
  DCHECK_EQ(MessageLoop::current(), host_context_.main_message_loop());
 
   host_ = NULL;
   if (state_ != kDisconnected) {
    SetState(kDisconnected);
  }
}
