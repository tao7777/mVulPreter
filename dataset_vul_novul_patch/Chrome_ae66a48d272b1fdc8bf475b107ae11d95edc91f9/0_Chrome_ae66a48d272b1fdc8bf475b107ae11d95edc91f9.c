NetworkChangeNotifier* NetworkChangeNotifier::Create() {
  if (g_network_change_notifier_factory)
    return g_network_change_notifier_factory->CreateInstance();

#if defined(OS_WIN)
  NetworkChangeNotifierWin* network_change_notifier =
      new NetworkChangeNotifierWin();
  network_change_notifier->WatchForAddressChange();
  return network_change_notifier;
#elif defined(OS_CHROMEOS)
   CHECK(false);
   return NULL;
 #elif defined(OS_LINUX) || defined(OS_ANDROID)
  return NetworkChangeNotifierLinux::Create();
 #elif defined(OS_MACOSX)
   return new NetworkChangeNotifierMac();
 #else
  NOTIMPLEMENTED();
  return NULL;
#endif
}
