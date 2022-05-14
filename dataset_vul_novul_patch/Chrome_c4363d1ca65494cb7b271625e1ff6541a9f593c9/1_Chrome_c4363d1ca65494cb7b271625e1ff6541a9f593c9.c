 void EventConverterEvdevImpl::OnFileCanReadWithoutBlocking(int fd) {
   input_event inputs[4];
   ssize_t read_size = read(fd, inputs, sizeof(inputs));
   if (read_size < 0) {
    if (errno == EINTR || errno == EAGAIN)
      return;
    if (errno != ENODEV)
      PLOG(ERROR) << "error reading device " << path_.value();
    Stop();
    return;
  }

  if (ignore_events_)
    return;

  DCHECK_EQ(read_size % sizeof(*inputs), 0u);
  ProcessEvents(inputs, read_size / sizeof(*inputs));
}
