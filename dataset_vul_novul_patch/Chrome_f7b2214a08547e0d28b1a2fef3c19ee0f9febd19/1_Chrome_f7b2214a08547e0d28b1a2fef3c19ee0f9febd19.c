bool GetNetworkList(NetworkInterfaceList* networks, int policy) {
  int s = socket(AF_INET, SOCK_DGRAM, 0);
  if (s <= 0) {
    PLOG(ERROR) << "socket";
    return false;
   }
  uint32_t num_ifs = 0;
  if (ioctl_netc_get_num_ifs(s, &num_ifs) < 0) {
    PLOG(ERROR) << "ioctl_netc_get_num_ifs";
    PCHECK(close(s) == 0);
    return false;
   }
 
  for (uint32_t i = 0; i < num_ifs; ++i) {
    netc_if_info_t interface;
 
    if (ioctl_netc_get_if_info_at(s, &i, &interface) < 0) {
      PLOG(WARNING) << "ioctl_netc_get_if_info_at";
      continue;
    }
 
    if (internal::IsLoopbackOrUnspecifiedAddress(
            reinterpret_cast<sockaddr*>(&(interface.addr)))) {
       continue;
    }
 
    IPEndPoint address;
    if (!address.FromSockAddr(reinterpret_cast<sockaddr*>(&(interface.addr)),
                              sizeof(interface.addr))) {
      DLOG(WARNING) << "ioctl_netc_get_if_info returned invalid address.";
       continue;
    }
 
    int prefix_length = 0;
    IPEndPoint netmask;
    if (netmask.FromSockAddr(reinterpret_cast<sockaddr*>(&(interface.netmask)),
                             sizeof(interface.netmask))) {
      prefix_length = MaskPrefixLength(netmask.address());
    }
 
     int attributes = 0;
 
    networks->push_back(
        NetworkInterface(interface.name, interface.name, interface.index,
                         NetworkChangeNotifier::CONNECTION_UNKNOWN,
                         address.address(), prefix_length, attributes));
   }
 
  PCHECK(close(s) == 0);
   return true;
 }
