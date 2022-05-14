NetworkChangeNotifierLinux::NetworkChangeNotifierLinux()
    : notifier_thread_(new Thread) {
  base::Thread::Options thread_options(MessageLoop::TYPE_IO, 0);
  notifier_thread_->StartWithOptions(thread_options);
}
