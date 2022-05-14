 void HostNPScriptObject::OnClientAuthenticated(const std::string& jid) {
  DCHECK_EQ(MessageLoop::current(), host_context_.main_message_loop());
 
   if (state_ == kDisconnecting) {
    return;
  }

  client_username_ = jid;
  size_t pos = client_username_.find('/');
  if (pos != std::string::npos)
    client_username_.replace(pos, std::string::npos, "");
  LOG(INFO) << "Client " << client_username_ << " connected.";
  SetState(kConnected);
 }
