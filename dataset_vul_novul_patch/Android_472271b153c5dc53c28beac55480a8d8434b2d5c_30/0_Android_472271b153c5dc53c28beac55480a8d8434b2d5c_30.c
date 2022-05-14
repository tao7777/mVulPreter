void btsock_l2cap_signaled(int fd, int flags, uint32_t user_id)
{
    l2cap_socket *sock;
 char drop_it = FALSE;

 /* We use MSG_DONTWAIT when sending data to JAVA, hence it can be accepted to hold the lock. */
    pthread_mutex_lock(&state_lock);
    sock = btsock_l2cap_find_by_id_l(user_id);
 if (sock) {
 if ((flags & SOCK_THREAD_FD_RD) && !sock->server) {

             if (sock->connected) {
                 int size = 0;
 
                if (!(flags & SOCK_THREAD_FD_EXCEPTION) || (TEMP_FAILURE_RETRY(ioctl(sock->our_fd, FIONREAD, &size))
                         == 0 && size)) {
                     uint8_t *buffer = osi_malloc(L2CAP_MAX_SDU_LENGTH);
 /* Apparently we hijack the req_id (UINT32) to pass the pointer to the buffer to
                     * the write complete callback, which call a free... wonder if this works on a
                     * 64 bit platform? */
 if (buffer != NULL) {
 /* The socket is created with SOCK_SEQPACKET, hence we read one message at
                         * the time. The maximum size of a message is allocated to ensure data is
                         * not lost. This is okay to do as Android uses virtual memory, hence even
                         * if we only use a fraction of the memory it should not block for others
                         * to use the memory. As the definition of ioctl(FIONREAD) do not clearly
                         * define what value will be returned if multiple messages are written to
                         * the socket before any message is read from the socket, we could
                         * potentially risk to allocate way more memory than needed. One of the use
                         * cases for this socket is obex where multiple 64kbyte messages are
                         * typically written to the socket in a tight loop, hence we risk the ioctl
                         * will return the total amount of data in the buffer, which could be
                         * multiple 64kbyte chunks.
                         * UPDATE: As bluedroid cannot handle 64kbyte buffers, the size is reduced
                         * to around 8kbyte - and using malloc for buffer allocation here seems to
                         * be wrong

                          * UPDATE: Since we are responsible for freeing the buffer in the
                          * write_complete_ind, it is OK to use malloc. */
 
                        int count = TEMP_FAILURE_RETRY(recv(fd, buffer, L2CAP_MAX_SDU_LENGTH,
                                MSG_NOSIGNAL | MSG_DONTWAIT));
                         APPL_TRACE_DEBUG("btsock_l2cap_signaled - %d bytes received from socket",
                                 count);
                         if (sock->fixed_chan) {
 if(BTA_JvL2capWriteFixed(sock->channel, (BD_ADDR*)&sock->addr,
 (UINT32)buffer, btsock_l2cap_cbk, buffer, count,
 (void *)user_id) != BTA_JV_SUCCESS) {
                                on_l2cap_write_fixed_done(buffer, user_id);
 }
 } else {
 if(BTA_JvL2capWrite(sock->handle, (UINT32)buffer, buffer, count,
 (void *)user_id) != BTA_JV_SUCCESS) {
                                on_l2cap_write_done(buffer, user_id);
 }
 }
 } else {
                        APPL_TRACE_ERROR("Unable to allocate memory for data packet from JAVA...")
 }
 }
 } else
                drop_it = TRUE;
 }
 if (flags & SOCK_THREAD_FD_WR) {
 if (flush_incoming_que_on_wr_signal_l(sock) && sock->connected)
                btsock_thread_add_fd(pth, sock->our_fd, BTSOCK_L2CAP, SOCK_THREAD_FD_WR, sock->id);

         }
         if (drop_it || (flags & SOCK_THREAD_FD_EXCEPTION)) {
             int size = 0;
            if (drop_it || TEMP_FAILURE_RETRY(ioctl(sock->our_fd, FIONREAD, &size)) != 0 || size == 0)
                 btsock_l2cap_free_l(sock);
         }
     }
    pthread_mutex_unlock(&state_lock);
}
