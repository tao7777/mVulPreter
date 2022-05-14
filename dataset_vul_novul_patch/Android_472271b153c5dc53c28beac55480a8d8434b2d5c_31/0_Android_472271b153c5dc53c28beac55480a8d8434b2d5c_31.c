static BOOLEAN flush_incoming_que_on_wr_signal_l(l2cap_socket *sock)
{
 uint8_t *buf;

     uint32_t len;
 
     while (packet_get_head_l(sock, &buf, &len)) {
        int sent = TEMP_FAILURE_RETRY(send(sock->our_fd, buf, len, MSG_DONTWAIT));
 
         if (sent == (signed)len)
             osi_free(buf);
 else if (sent >= 0) {
            packet_put_head_l(sock, buf + sent, len - sent);
            osi_free(buf);
 if (!sent) /* special case if other end not keeping up */
 return TRUE;
 }
 else {
            packet_put_head_l(sock, buf, len);
            osi_free(buf);
 return errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN;
 }
 }

 return FALSE;
}
