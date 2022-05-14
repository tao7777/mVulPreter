NetworkChangeNotifierLinux::NetworkChangeNotifierLinux()
NetworkChangeNotifierLinux* NetworkChangeNotifierLinux::Create() {
  return new NetworkChangeNotifierLinux(NULL);
}

NetworkChangeNotifierLinux* NetworkChangeNotifierLinux::CreateForTest(
    dbus::Bus* bus) {
  return new NetworkChangeNotifierLinux(bus);
}

NetworkChangeNotifierLinux::NetworkChangeNotifierLinux(dbus::Bus* bus)
    : notifier_thread_(new Thread(bus)) {
  base::Thread::Options thread_options(MessageLoop::TYPE_IO, 0);
  notifier_thread_->StartWithOptions(thread_options);
}
