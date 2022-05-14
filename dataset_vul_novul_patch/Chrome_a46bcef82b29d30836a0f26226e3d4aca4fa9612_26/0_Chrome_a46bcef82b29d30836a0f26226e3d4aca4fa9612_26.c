 void HostNPScriptObject::OnShutdownFinished() {
  if (MessageLoop::current() != host_context_.main_message_loop()) {
    host_context_.main_message_loop()->PostTask(FROM_HERE, base::Bind(
        &HostNPScriptObject::OnShutdownFinished, base::Unretained(this)));
    return;
  }
 
   disconnected_event_.Signal();
 }
