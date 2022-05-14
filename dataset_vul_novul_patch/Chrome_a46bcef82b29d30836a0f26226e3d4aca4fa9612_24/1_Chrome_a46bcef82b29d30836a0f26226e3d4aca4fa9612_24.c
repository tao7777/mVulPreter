 void HostNPScriptObject::OnClientDisconnected(const std::string& jid) {
  DCHECK_EQ(MessageLoop::current(), host_context_.main_message_loop());
 
   client_username_.clear();
 
  DisconnectInternal();
 }
