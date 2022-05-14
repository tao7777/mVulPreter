   void RemoveClientSession() {
    context_.network_message_loop()->PostTask(
        FROM_HERE, base::Bind(
            &ClientSession::OnConnectionClosed, client_, connection_));
   }
