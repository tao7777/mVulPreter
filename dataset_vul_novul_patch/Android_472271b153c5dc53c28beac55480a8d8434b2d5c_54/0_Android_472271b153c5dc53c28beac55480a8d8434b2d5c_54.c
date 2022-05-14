static uint16_t transmit_data_on(int fd, uint8_t *data, uint16_t length) {
  assert(data != NULL);
  assert(length > 0);

 
   uint16_t transmitted_length = 0;
   while (length > 0) {
    ssize_t ret = TEMP_FAILURE_RETRY(write(fd, data + transmitted_length, length));
     switch (ret) {
       case -1:
         LOG_ERROR("In %s, error writing to the serial port with fd %d: %s", __func__, fd, strerror(errno));
 return transmitted_length;
 case 0:
 return transmitted_length;
 default:
        transmitted_length += ret;
        length -= ret;
 break;
 }
 }

 return transmitted_length;
}
