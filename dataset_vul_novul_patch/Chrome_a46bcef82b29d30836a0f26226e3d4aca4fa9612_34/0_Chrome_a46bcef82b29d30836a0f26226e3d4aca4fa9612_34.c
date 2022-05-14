 ConnectionToClient::~ConnectionToClient() {
  if (session_.get())
    message_loop_->DeleteSoon(FROM_HERE, session_.release());
 }
