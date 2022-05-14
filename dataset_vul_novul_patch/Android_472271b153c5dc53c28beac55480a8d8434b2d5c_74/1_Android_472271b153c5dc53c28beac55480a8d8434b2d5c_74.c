static void uipc_flush_ch_locked(tUIPC_CH_ID ch_id)
{
 char buf[UIPC_FLUSH_BUFFER_SIZE];
 struct pollfd pfd;
 int ret;

    pfd.events = POLLIN;
    pfd.fd = uipc_main.ch[ch_id].fd;

 if (uipc_main.ch[ch_id].fd == UIPC_DISCONNECTED)
 {
        BTIF_TRACE_EVENT("%s() - fd disconnected. Exiting", __FUNCTION__);
 return;
 }

 
     while (1)
     {
        ret = poll(&pfd, 1, 1);
         BTIF_TRACE_VERBOSE("%s() - polling fd %d, revents: 0x%x, ret %d",
                 __FUNCTION__, pfd.fd, pfd.revents, ret);
 
 if (pfd.revents & (POLLERR|POLLHUP))
 {
            BTIF_TRACE_EVENT("%s() - POLLERR or POLLHUP. Exiting", __FUNCTION__);
 return;
 }

 if (ret <= 0)
 {
            BTIF_TRACE_EVENT("%s() - error (%d). Exiting", __FUNCTION__, ret);
 return;
 }

 
         /* read sufficiently large buffer to ensure flush empties socket faster than
            it is getting refilled */
        read(pfd.fd, &buf, UIPC_FLUSH_BUFFER_SIZE);
     }
 }
