int UDPSocketWin::DoBind(const IPEndPoint& address) {
  SockaddrStorage storage;
  if (!address.ToSockAddr(storage.addr, &storage.addr_len))
    return ERR_ADDRESS_INVALID;
  int rv = bind(socket_, storage.addr, storage.addr_len);
  if (rv == 0)
    return OK;
   int last_error = WSAGetLastError();
   UMA_HISTOGRAM_SPARSE_SLOWLY("Net.UdpSocketBindErrorFromWinOS", last_error);
  if (last_error == WSAEACCES || last_error == WSAEINVAL)
     return ERR_ADDRESS_IN_USE;
   return MapSystemError(last_error);
 }
