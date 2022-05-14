void ChromotingHost::LocalMouseMoved(const SkIPoint& new_pos) {
  if (!context_->network_message_loop()->BelongsToCurrentThread()) {
    context_->network_message_loop()->PostTask(
         FROM_HERE, base::Bind(&ChromotingHost::LocalMouseMoved, this, new_pos));
     return;
   }

   ClientList::iterator client;
   for (client = clients_.begin(); client != clients_.end(); ++client) {
     client->get()->LocalMouseMoved(new_pos);
   }
 }
