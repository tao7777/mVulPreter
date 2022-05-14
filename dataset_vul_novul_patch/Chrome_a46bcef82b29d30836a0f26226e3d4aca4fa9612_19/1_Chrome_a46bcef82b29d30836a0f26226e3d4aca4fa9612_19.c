 void ClientSession::Disconnect() {
  connection_->Disconnect();
   authenticated_ = false;
   RestoreEventState();
 }
