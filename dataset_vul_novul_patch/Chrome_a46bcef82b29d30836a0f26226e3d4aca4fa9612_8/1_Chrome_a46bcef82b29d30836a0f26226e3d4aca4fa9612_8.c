 void ChromotingHost::OnSessionSequenceNumber(ClientSession* session,
                                              int64 sequence_number) {
  if (MessageLoop::current() != context_->main_message_loop()) {
    context_->main_message_loop()->PostTask(
        FROM_HERE, base::Bind(&ChromotingHost::OnSessionSequenceNumber, this,
                              make_scoped_refptr(session), sequence_number));
    return;
  }
   if (recorder_.get())
     recorder_->UpdateSequenceNumber(sequence_number);
 }
