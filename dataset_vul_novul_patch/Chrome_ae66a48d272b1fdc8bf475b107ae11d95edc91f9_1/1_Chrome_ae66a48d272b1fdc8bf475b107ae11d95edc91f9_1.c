void NetworkChangeNotifierLinux::Thread::Init() {
   resolv_file_watcher_.reset(new FilePathWatcher);
   hosts_file_watcher_.reset(new FilePathWatcher);
   file_watcher_delegate_ = new DNSWatchDelegate(base::Bind(
      &NetworkChangeNotifierLinux::Thread::NotifyObserversOfDNSChange,
      base::Unretained(this)));
   if (!resolv_file_watcher_->Watch(
           FilePath(FILE_PATH_LITERAL("/etc/resolv.conf")),
           file_watcher_delegate_.get())) {
    LOG(ERROR) << "Failed to setup watch for /etc/resolv.conf";
  }
  if (!hosts_file_watcher_->Watch(FilePath(FILE_PATH_LITERAL("/etc/hosts")),
          file_watcher_delegate_.get())) {
    LOG(ERROR) << "Failed to setup watch for /etc/hosts";
  }
  netlink_fd_ = InitializeNetlinkSocket();
  if (netlink_fd_ < 0) {
    netlink_fd_ = kInvalidSocket;
     return;
   }
   ListenForNotifications();
 }
