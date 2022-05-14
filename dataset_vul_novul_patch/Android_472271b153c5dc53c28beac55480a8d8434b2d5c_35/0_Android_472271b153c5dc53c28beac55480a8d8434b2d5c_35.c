static sent_status_t send_data_to_app(int fd, BT_HDR *p_buf) {

   if (p_buf->len == 0)
     return SENT_ALL;
 
  ssize_t sent = TEMP_FAILURE_RETRY(send(fd, p_buf->data + p_buf->offset, p_buf->len, MSG_DONTWAIT));
 
   if (sent == -1) {
     if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
 return SENT_NONE;
    LOG_ERROR("%s error writing RFCOMM data back to app: %s", __func__, strerror(errno));
 return SENT_FAILED;
 }

 if (sent == 0)
 return SENT_FAILED;

 if (sent == p_buf->len)
 return SENT_ALL;

  p_buf->offset += sent;
  p_buf->len -= sent;
 return SENT_PARTIAL;
}
