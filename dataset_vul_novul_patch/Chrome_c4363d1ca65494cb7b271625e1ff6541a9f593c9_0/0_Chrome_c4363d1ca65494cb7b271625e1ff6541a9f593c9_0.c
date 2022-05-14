 void EventReaderLibevdevCros::OnFileCanReadWithoutBlocking(int fd) {
  TRACE_EVENT1("evdev", "EventReaderLibevdevCros::OnFileCanReadWithoutBlocking",
               "fd", fd);

   if (EvdevRead(&evdev_)) {
     if (errno == EINTR || errno == EAGAIN)
       return;
    if (errno != ENODEV)
      PLOG(ERROR) << "error reading device " << path_.value();
    Stop();
    return;
  }
}
