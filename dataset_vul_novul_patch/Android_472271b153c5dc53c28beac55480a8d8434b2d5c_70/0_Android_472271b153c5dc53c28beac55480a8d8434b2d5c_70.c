UINT32 UIPC_Read(tUIPC_CH_ID ch_id, UINT16 *p_msg_evt, UINT8 *p_buf, UINT32 len)
{
 int n;
 int n_read = 0;
 int fd = uipc_main.ch[ch_id].fd;
 struct pollfd pfd;
    UNUSED(p_msg_evt);

 if (ch_id >= UIPC_CH_NUM)
 {
        BTIF_TRACE_ERROR("UIPC_Read : invalid ch id %d", ch_id);
 return 0;
 }

 if (fd == UIPC_DISCONNECTED)
 {
        BTIF_TRACE_ERROR("UIPC_Read : channel %d closed", ch_id);
 return 0;
 }


 while (n_read < (int)len)
 {
        pfd.fd = fd;
        pfd.events = POLLIN|POLLHUP;

 
         /* make sure there is data prior to attempting read to avoid blocking
            a read for more than poll timeout */
        if (TEMP_FAILURE_RETRY(poll(&pfd, 1, uipc_main.ch[ch_id].read_poll_tmo_ms)) == 0)
         {
             BTIF_TRACE_EVENT("poll timeout (%d ms)", uipc_main.ch[ch_id].read_poll_tmo_ms);
             break;
 }


 if (pfd.revents & (POLLHUP|POLLNVAL) )
 {
            BTIF_TRACE_EVENT("poll : channel detached remotely");
            UIPC_LOCK();
            uipc_close_locked(ch_id);
            UIPC_UNLOCK();

             return 0;
         }
 
        n = TEMP_FAILURE_RETRY(recv(fd, p_buf+n_read, len-n_read, 0));
 
 
 if (n == 0)
 {
            BTIF_TRACE_EVENT("UIPC_Read : channel detached remotely");
            UIPC_LOCK();
            uipc_close_locked(ch_id);
            UIPC_UNLOCK();
 return 0;
 }

 if (n < 0)
 {
            BTIF_TRACE_EVENT("UIPC_Read : read failed (%s)", strerror(errno));
 return 0;
 }

        n_read+=n;

 }

 return n_read;
}
