 void ClientSession::Disconnect() {
  DCHECK(connection_);
   authenticated_ = false;
   RestoreEventState();

  // This triggers OnSessionClosed() and the session may be destroyed
  // as the result, so this call must be the last in this method.
  connection_->Disconnect();
 }
