 void TouchEventConverterEvdev::OnFileCanReadWithoutBlocking(int fd) {
  TRACE_EVENT1("evdev",
               "TouchEventConverterEvdev::OnFileCanReadWithoutBlocking", "fd",
               fd);

   input_event inputs[kNumTouchEvdevSlots * 6 + 1];
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

  for (unsigned i = 0; i < read_size / sizeof(*inputs); i++) {
    if (!has_mt_) {
      EmulateMultitouchEvent(inputs[i]);
    }

    ProcessMultitouchEvent(inputs[i]);
  }
}
