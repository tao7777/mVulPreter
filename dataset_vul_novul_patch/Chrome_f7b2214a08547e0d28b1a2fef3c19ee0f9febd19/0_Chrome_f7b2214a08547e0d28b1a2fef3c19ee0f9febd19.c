bool GetNetworkList(NetworkInterfaceList* networks, int policy) {
IPAddress NetAddressToIPAddress(const netstack::NetAddress& addr) {
  if (addr.ipv4) {
    return IPAddress(addr.ipv4->addr.data(), addr.ipv4->addr.count());
   }
  if (addr.ipv6) {
    return IPAddress(addr.ipv6->addr.data(), addr.ipv6->addr.count());
   }
  return IPAddress();
}
 
bool GetNetworkList(NetworkInterfaceList* networks, int policy) {
  netstack::NetstackSyncPtr netstack =
      base::fuchsia::ComponentContext::GetDefault()
          ->ConnectToServiceSync<netstack::Netstack>();
 
  fidl::VectorPtr<netstack::NetInterface> interfaces;
  if (!netstack->GetInterfaces(&interfaces))
    return false;
 
  for (auto& interface : interfaces.get()) {
    // Check if the interface is up.
    if (!(interface.flags & netstack::NetInterfaceFlagUp))
       continue;
 
    // Skip loopback.
    if (interface.features & netstack::interfaceFeatureLoopback)
       continue;
 
    NetworkChangeNotifier::ConnectionType connection_type =
        (interface.features & netstack::interfaceFeatureWlan)
            ? NetworkChangeNotifier::CONNECTION_WIFI
            : NetworkChangeNotifier::CONNECTION_UNKNOWN;
 
    // addresses. Currently Netstack doesn't provide this information.
     int attributes = 0;
 
    networks->push_back(NetworkInterface(
        *interface.name, *interface.name, interface.id, connection_type,
        NetAddressToIPAddress(interface.addr),
        MaskPrefixLength(NetAddressToIPAddress(interface.netmask)),
        attributes));
   }
 
   return true;
 }
