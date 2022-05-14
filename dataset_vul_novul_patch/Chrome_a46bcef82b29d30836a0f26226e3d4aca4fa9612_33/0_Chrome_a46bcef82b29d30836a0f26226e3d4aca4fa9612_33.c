 void ConnectionToClient::Disconnect() {
  DCHECK(message_loop_->BelongsToCurrentThread());
 
   CloseChannels();
 
  DCHECK(session_.get());
  Session* session = session_.release();

  // It may not be safe to delete |session_| here becase this method
  // may be invoked in resonse to a libjingle event and libjingle's
  // sigslot doesn't handle it properly, so postpone the deletion.
  message_loop_->DeleteSoon(FROM_HERE, session);

  // This should trigger OnConnectionClosed() event and this object
  // may be destroyed as the result.
  session->Close();
 }
