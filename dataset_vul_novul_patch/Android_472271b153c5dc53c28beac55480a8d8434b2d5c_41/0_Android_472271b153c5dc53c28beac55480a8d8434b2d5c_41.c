 int btsock_thread_wakeup(int h)
 {
 if(h < 0 || h >= MAX_THREAD)
 {
        APPL_TRACE_ERROR("invalid bt thread handle:%d", h);
 return FALSE;
 }
 if(ts[h].cmd_fdw == -1)
 {
        APPL_TRACE_ERROR("thread handle:%d, cmd socket is not created", h);

         return FALSE;
     }
     sock_cmd_t cmd = {CMD_WAKEUP, 0, 0, 0, 0};
    return TEMP_FAILURE_RETRY(send(ts[h].cmd_fdw, &cmd, sizeof(cmd), 0)) == sizeof(cmd);
 }
