 void HostNPScriptObject::OnShutdown() {
  if (MessageLoop::current() != host_context_.main_message_loop()) {
    host_context_.main_message_loop()->PostTask(FROM_HERE, base::Bind(
        &HostNPScriptObject::OnShutdown, base::Unretained(this)));
    return;
  }
 
   host_ = NULL;
   if (state_ != kDisconnected) {
    SetState(kDisconnected);
  }
}
