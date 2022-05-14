NetworkChangeNotifierLinux::Thread::Thread()
NetworkChangeNotifierLinux::Thread::Thread(dbus::Bus* bus)
     : base::Thread("NetworkChangeNotifier"),
       netlink_fd_(kInvalidSocket),
      ALLOW_THIS_IN_INITIALIZER_LIST(ptr_factory_(this)),
      network_manager_api_(
          base::Bind(&NetworkChangeNotifierLinux::Thread
                     ::NotifyObserversOfOnlineStateChange),
          bus) {
 }
