 void ConnectionToClient::Disconnect() {
  if (!message_loop_->BelongsToCurrentThread()) {
    message_loop_->PostTask(
        FROM_HERE, base::Bind(&ConnectionToClient::Disconnect, this));
    return;
  }
 
   CloseChannels();
 
  if (session_.get())
    session_.reset();
 }
