static void *sock_poll_thread(void *arg)
{
 struct pollfd pfds[MAX_POLL];
    memset(pfds, 0, sizeof(pfds));
 int h = (intptr_t)arg;

     for(;;)
     {
         prepare_poll_fds(h, pfds);
        int ret = TEMP_FAILURE_RETRY(poll(pfds, ts[h].poll_count, -1));
         if(ret == -1)
         {
             APPL_TRACE_ERROR("poll ret -1, exit the thread, errno:%d, err:%s", errno, strerror(errno));
 break;
 }
 if(ret != 0)
 {
 int need_process_data_fd = TRUE;
 if(pfds[0].revents) //cmd fd always is the first one
 {
                asrt(pfds[0].fd == ts[h].cmd_fdr);
 if(!process_cmd_sock(h))
 {
                    APPL_TRACE_DEBUG("h:%d, process_cmd_sock return false, exit...", h);
 break;
 }
 if(ret == 1)
                    need_process_data_fd = FALSE;
 else ret--; //exclude the cmd fd
 }
 if(need_process_data_fd)
                process_data_sock(h, pfds, ret);
 }
 else {APPL_TRACE_DEBUG("no data, select ret: %d", ret)};
 }
    ts[h].thread_id = -1;
    APPL_TRACE_DEBUG("socket poll thread exiting, h:%d", h);
 return 0;
}
