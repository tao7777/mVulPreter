 int SocketStream::DoBeforeConnect() {
   next_state_ = STATE_BEFORE_CONNECT_COMPLETE;
  if (!context_ || !context_->network_delegate())
     return OK;
 
   int result = context_->network_delegate()->NotifyBeforeSocketStreamConnect(
       this, io_callback_);
  if (result != OK && result != ERR_IO_PENDING)
    next_state_ = STATE_CLOSE;

  return result;
}
