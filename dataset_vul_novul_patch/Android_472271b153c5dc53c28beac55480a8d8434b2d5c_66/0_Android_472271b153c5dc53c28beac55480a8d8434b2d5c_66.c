 static void msleep(uint64_t ms) {
  TEMP_FAILURE_RETRY(usleep(ms * 1000));
 }
