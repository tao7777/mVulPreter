int UDPSocketLibevent::InternalConnect(const IPEndPoint& address) {
  DCHECK(CalledOnValidThread());
  DCHECK(!is_connected());
  DCHECK(!remote_address_.get());
  int addr_family = address.GetSockAddrFamily();
  int rv = CreateSocket(addr_family);
  if (rv < 0)
    return rv;

  if (bind_type_ == DatagramSocket::RANDOM_BIND) {
    size_t addr_size =
        addr_family == AF_INET ? kIPv4AddressSize : kIPv6AddressSize;
    IPAddressNumber addr_any(addr_size);
    rv = RandomBind(addr_any);
  }
 
   if (rv < 0) {
    UMA_HISTOGRAM_SPARSE_SLOWLY("Net.UdpSocketRandomBindErrorCode", -rv);
     Close();
     return rv;
   }

  SockaddrStorage storage;
  if (!address.ToSockAddr(storage.addr, &storage.addr_len)) {
    Close();
    return ERR_ADDRESS_INVALID;
  }

  rv = HANDLE_EINTR(connect(socket_, storage.addr, storage.addr_len));
  if (rv < 0) {
    int result = MapSystemError(errno);
    Close();
    return result;
  }

  remote_address_.reset(new IPEndPoint(address));
  return rv;
}
