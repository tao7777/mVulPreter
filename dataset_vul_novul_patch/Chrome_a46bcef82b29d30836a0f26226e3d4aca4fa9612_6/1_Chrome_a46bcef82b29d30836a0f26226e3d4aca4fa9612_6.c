 void ChromotingHost::OnSessionAuthenticated(ClientSession* client) {
   DCHECK(context_->network_message_loop()->BelongsToCurrentThread());
  protocol::Session* session = client->connection()->session();
  context_->main_message_loop()->PostTask(
      FROM_HERE, base::Bind(&ChromotingHost::AddAuthenticatedClient,
                            this, make_scoped_refptr(client),
                            session->config(), session->jid()));
 }
