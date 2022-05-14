 bool btsock_thread_remove_fd_and_close(int thread_handle, int fd)
{
 if (thread_handle < 0 || thread_handle >= MAX_THREAD)
 {
        APPL_TRACE_ERROR("%s invalid thread handle: %d", __func__, thread_handle);
 return false;
 }
 if (fd == -1)
 {
        APPL_TRACE_ERROR("%s invalid file descriptor.", __func__);
 return false;

     }
 
     sock_cmd_t cmd = {CMD_REMOVE_FD, fd, 0, 0, 0};
    return TEMP_FAILURE_RETRY(send(ts[thread_handle].cmd_fdw, &cmd, sizeof(cmd), 0)) == sizeof(cmd);
 }
