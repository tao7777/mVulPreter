 int btsock_thread_post_cmd(int h, int type, const unsigned char* data, int size, uint32_t user_id)
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
 sock_cmd_t cmd = {CMD_USER_PRIVATE, 0, type, size, user_id};
    APPL_TRACE_DEBUG("post cmd type:%d, size:%d, h:%d, ", type, size, h);
 sock_cmd_t* cmd_send = &cmd;
 int size_send = sizeof(cmd);
 if(data && size)
 {
        size_send = sizeof(cmd) + size;
        cmd_send = (sock_cmd_t*)alloca(size_send);
 if(cmd_send)
 {
 *cmd_send = cmd;
            memcpy(cmd_send + 1, data, size);
 }
 else
 {
            APPL_TRACE_ERROR("alloca failed at h:%d, cmd type:%d, size:%d", h, type, size_send);

             return FALSE;
         }
     }
    return send(ts[h].cmd_fdw, cmd_send, size_send, 0) == size_send;
 }
