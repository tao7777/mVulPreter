 void P2PSocketDispatcherHost::OnAcceptIncomingTcpConnection(
     const IPC::Message& msg, int listen_socket_id,
    net::IPEndPoint remote_address, int connected_socket_id) {
   P2PSocketHost* socket = LookupSocket(msg.routing_id(), listen_socket_id);
   if (!socket) {
     LOG(ERROR) << "Received P2PHostMsg_AcceptIncomingTcpConnection "
        "for invalid socket_id.";
    return;
  }
  P2PSocketHost* accepted_connection =
      socket->AcceptIncomingTcpConnection(remote_address, connected_socket_id);
  if (accepted_connection) {
    sockets_.insert(std::pair<ExtendedSocketId, P2PSocketHost*>(
        ExtendedSocketId(msg.routing_id(), connected_socket_id),
        accepted_connection));
  }
}
