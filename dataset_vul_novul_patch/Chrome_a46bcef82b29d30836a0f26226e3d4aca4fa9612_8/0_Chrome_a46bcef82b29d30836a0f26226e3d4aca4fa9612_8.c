 void ChromotingHost::OnSessionSequenceNumber(ClientSession* session,
                                              int64 sequence_number) {
  DCHECK(context_->network_message_loop()->BelongsToCurrentThread());
   if (recorder_.get())
     recorder_->UpdateSequenceNumber(sequence_number);
 }
