 int btsock_thread_exit(int h)
 {
 if(h < 0 || h >= MAX_THREAD)
 {
        APPL_TRACE_ERROR("invalid bt thread handle:%d", h);
 return FALSE;
 }
 if(ts[h].cmd_fdw == -1)
 {
        APPL_TRACE_ERROR("cmd socket is not created");

         return FALSE;
     }
     sock_cmd_t cmd = {CMD_EXIT, 0, 0, 0, 0};
    if(TEMP_FAILURE_RETRY(send(ts[h].cmd_fdw, &cmd, sizeof(cmd), 0)) == sizeof(cmd))
     {
         pthread_join(ts[h].thread_id, 0);
         pthread_mutex_lock(&thread_slot_lock);
        free_thread_slot(h);
        pthread_mutex_unlock(&thread_slot_lock);
 return TRUE;
 }
 return FALSE;
}
