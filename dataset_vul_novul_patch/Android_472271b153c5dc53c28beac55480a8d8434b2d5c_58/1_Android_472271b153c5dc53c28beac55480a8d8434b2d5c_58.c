static void inbound_data_waiting(void *context) {
 eager_reader_t *reader = (eager_reader_t *)context;

 data_buffer_t *buffer = (data_buffer_t *)reader->allocator->alloc(reader->buffer_size + sizeof(data_buffer_t));
 if (!buffer) {
    LOG_ERROR("%s couldn't aquire memory for inbound data buffer.", __func__);
 return;
 }


   buffer->length = 0;
   buffer->offset = 0;
 
  int bytes_read = read(reader->inbound_fd, buffer->data, reader->buffer_size);
   if (bytes_read > 0) {
     buffer->length = bytes_read;
    fixed_queue_enqueue(reader->buffers, buffer);

    eventfd_write(reader->bytes_available_fd, bytes_read);
 } else {
 if (bytes_read == 0)
      LOG_WARN("%s fd said bytes existed, but none were found.", __func__);
 else
      LOG_WARN("%s unable to read from file descriptor: %s", __func__, strerror(errno));

    reader->allocator->free(buffer);
 }
}
