 void ChromotingHost::OnSessionClosed(ClientSession* client) {
   DCHECK(context_->network_message_loop()->BelongsToCurrentThread());
 
  VLOG(1) << "Connection to client closed.";
  context_->main_message_loop()->PostTask(
      FROM_HERE, base::Bind(&ChromotingHost::OnClientDisconnected, this,
                            make_scoped_refptr(client)));
 }
