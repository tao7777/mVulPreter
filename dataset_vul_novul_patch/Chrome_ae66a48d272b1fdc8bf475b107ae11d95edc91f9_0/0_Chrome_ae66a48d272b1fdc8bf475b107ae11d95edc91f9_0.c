 void NetworkChangeNotifierLinux::Thread::CleanUp() {
  if (netlink_fd_ != kInvalidSocket) {
    if (HANDLE_EINTR(close(netlink_fd_)) != 0)
      PLOG(ERROR) << "Failed to close socket";
    netlink_fd_ = kInvalidSocket;
    netlink_watcher_.StopWatchingFileDescriptor();
  }
   resolv_file_watcher_.reset();
   hosts_file_watcher_.reset();

  network_manager_api_.CleanUp();
 }
