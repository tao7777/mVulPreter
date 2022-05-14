 void ClientSession::OnConnectionFailed(
    protocol::ConnectionToClient* connection) {
  DCHECK_EQ(connection_.get(), connection);
   scoped_refptr<ClientSession> self = this;
   event_handler_->OnSessionClosed(this);
 }
