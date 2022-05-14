void GKI_delay(UINT32 timeout_ms) {
 struct timespec delay;
  delay.tv_sec = timeout_ms / 1000;
  delay.tv_nsec = 1000 * 1000 * (timeout_ms % 1000);

 
   int err;
   do {
    err = nanosleep(&delay, &delay);
   } while (err == -1 && errno == EINTR);
 }
