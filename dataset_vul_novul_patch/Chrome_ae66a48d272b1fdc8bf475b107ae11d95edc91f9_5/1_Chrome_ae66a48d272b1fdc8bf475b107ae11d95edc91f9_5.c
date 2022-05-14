NetworkChangeNotifierLinux::Thread::Thread()
     : base::Thread("NetworkChangeNotifier"),
       netlink_fd_(kInvalidSocket),
      ALLOW_THIS_IN_INITIALIZER_LIST(ptr_factory_(this)) {
 }
