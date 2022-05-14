static void *btif_hh_poll_event_thread(void *arg)
{
 btif_hh_device_t *p_dev = arg;
    APPL_TRACE_DEBUG("%s: Thread created fd = %d", __FUNCTION__, p_dev->fd);
 struct pollfd pfds[1];
 int ret;

    pfds[0].fd = p_dev->fd;
    pfds[0].events = POLLIN;


     uhid_set_non_blocking(p_dev->fd);
 
     while(p_dev->hh_keep_polling){
        ret = TEMP_FAILURE_RETRY(poll(pfds, 1, 50));
         if (ret < 0) {
             APPL_TRACE_ERROR("%s: Cannot poll for fds: %s\n", __FUNCTION__, strerror(errno));
             break;
 }
 if (pfds[0].revents & POLLIN) {
            APPL_TRACE_DEBUG("btif_hh_poll_event_thread: POLLIN");
            ret = uhid_event(p_dev);
 if (ret){
 break;
 }
 }
 }

    p_dev->hh_poll_thread_id = -1;
 return 0;
}
