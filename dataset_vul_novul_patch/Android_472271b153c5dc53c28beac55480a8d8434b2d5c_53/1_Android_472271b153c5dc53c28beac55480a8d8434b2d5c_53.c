static uint16_t transmit_data(serial_data_type_t type, uint8_t *data, uint16_t length) {
  assert(data != NULL);
  assert(length > 0);

 if (type < DATA_TYPE_COMMAND || type > DATA_TYPE_SCO) {
    LOG_ERROR("%s invalid data type: %d", __func__, type);
 return 0;
 }

 --data;
 uint8_t previous_byte = *data;
 *(data) = type;
 ++length;

 
   uint16_t transmitted_length = 0;
   while (length > 0) {
    ssize_t ret = write(uart_fd, data + transmitted_length, length);
     switch (ret) {
       case -1:
         LOG_ERROR("In %s, error writing to the uart serial port: %s", __func__, strerror(errno));
 goto done;
 case 0:
 goto done;
 default:
        transmitted_length += ret;
        length -= ret;
 break;
 }
 }

done:;
 *(data) = previous_byte;

 if (transmitted_length > 0)
 --transmitted_length;

 return transmitted_length;
}
