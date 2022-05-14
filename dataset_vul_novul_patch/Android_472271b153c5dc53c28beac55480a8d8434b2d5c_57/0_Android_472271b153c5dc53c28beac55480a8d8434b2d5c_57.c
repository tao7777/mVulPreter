static bool has_byte(const eager_reader_t *reader) {
  assert(reader != NULL);

  fd_set read_fds;
  FD_ZERO(&read_fds);
  FD_SET(reader->bytes_available_fd, &read_fds);

 struct timeval timeout;

   timeout.tv_sec = 0;
   timeout.tv_usec = 0;
 
  TEMP_FAILURE_RETRY(select(reader->bytes_available_fd + 1, &read_fds, NULL, NULL, &timeout));
   return FD_ISSET(reader->bytes_available_fd, &read_fds);
 }
