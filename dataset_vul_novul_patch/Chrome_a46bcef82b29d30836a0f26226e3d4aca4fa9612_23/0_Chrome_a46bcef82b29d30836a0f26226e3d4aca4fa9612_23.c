 void HostNPScriptObject::OnClientAuthenticated(const std::string& jid) {
  if (MessageLoop::current() != host_context_.main_message_loop()) {
    host_context_.main_message_loop()->PostTask(FROM_HERE, base::Bind(
        &HostNPScriptObject::OnClientAuthenticated,
        base::Unretained(this), jid));
    return;
  }
 
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
