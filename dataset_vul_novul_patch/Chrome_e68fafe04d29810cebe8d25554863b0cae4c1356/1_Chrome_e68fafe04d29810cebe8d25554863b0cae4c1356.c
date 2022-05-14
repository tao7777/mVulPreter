int UDPSocketLibevent::DoBind(const IPEndPoint& address) {
  SockaddrStorage storage;
  if (!address.ToSockAddr(storage.addr, &storage.addr_len))
    return ERR_ADDRESS_INVALID;
  int rv = bind(socket_, storage.addr, storage.addr_len);
  if (rv == 0)
     return OK;
   int last_error = errno;
   UMA_HISTOGRAM_SPARSE_SLOWLY("Net.UdpSocketBindErrorFromPosix", last_error);
   return MapSystemError(last_error);
 }
