 void HostNPScriptObject::OnShutdownFinished() {
  DCHECK_EQ(MessageLoop::current(), host_context_.main_message_loop());
 
   disconnected_event_.Signal();
 }
