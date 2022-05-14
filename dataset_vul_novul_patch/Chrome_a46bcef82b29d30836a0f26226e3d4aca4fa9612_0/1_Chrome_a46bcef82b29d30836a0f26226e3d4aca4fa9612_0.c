 int ChromotingHost::AuthenticatedClientsCount() const {
   int authenticated_clients = 0;
   for (ClientList::const_iterator it = clients_.begin(); it != clients_.end();
        ++it) {
    if (it->get()->authenticated())
      ++authenticated_clients;
  }
  return authenticated_clients;
}
