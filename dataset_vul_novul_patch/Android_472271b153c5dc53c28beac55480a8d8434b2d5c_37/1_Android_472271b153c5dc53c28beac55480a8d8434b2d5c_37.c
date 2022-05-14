int btsock_thread_add_fd(int h, int fd, int type, int flags, uint32_t user_id)
{
 if(h < 0 || h >= MAX_THREAD)
 {
        APPL_TRACE_ERROR("invalid bt thread handle:%d", h);
 return FALSE;
 }
 if(ts[h].cmd_fdw == -1)
 {
        APPL_TRACE_ERROR("cmd socket is not created. socket thread may not initialized");
 return FALSE;
 }
 if(flags & SOCK_THREAD_ADD_FD_SYNC)
 {
 if(ts[h].thread_id == pthread_self())
 {
            flags &= ~SOCK_THREAD_ADD_FD_SYNC;
            add_poll(h, fd, type, flags, user_id);
 return TRUE;
 }
        APPL_TRACE_DEBUG("THREAD_ADD_FD_SYNC is not called in poll thread, fallback to async");

     }
     sock_cmd_t cmd = {CMD_ADD_FD, fd, type, flags, user_id};
     APPL_TRACE_DEBUG("adding fd:%d, flags:0x%x", fd, flags);
    return send(ts[h].cmd_fdw, &cmd, sizeof(cmd), 0) == sizeof(cmd);
 }
