bool GetNetworkList(NetworkInterfaceList* networks, int policy) {
  if (networks == NULL)
    return false;

  base::ThreadRestrictions::AssertIOAllowed();

  ifaddrs* interfaces;
  if (getifaddrs(&interfaces) < 0) {
    PLOG(ERROR) << "getifaddrs";
    return false;
  }

   std::unique_ptr<internal::IPAttributesGetter> ip_attributes_getter;
 
 #if defined(OS_MACOSX) && !defined(OS_IOS)
  ip_attributes_getter = std::make_unique<internal::IPAttributesGetterMac>();
 #endif
 
   bool result = internal::IfaddrsToNetworkInterfaceList(
      policy, interfaces, ip_attributes_getter.get(), networks);
  freeifaddrs(interfaces);
  return result;
}
