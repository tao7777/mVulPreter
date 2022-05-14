static void uipc_read_task(void *arg)
{
 int ch_id;
 int result;
    UNUSED(arg);

    prctl(PR_SET_NAME, (unsigned long)"uipc-main", 0, 0, 0);

    raise_priority_a2dp(TASK_UIPC_READ);

 while (uipc_main.running)

     {
         uipc_main.read_set = uipc_main.active_set;
 
        result = select(uipc_main.max_fd+1, &uipc_main.read_set, NULL, NULL, NULL);
 
         if (result == 0)
         {
            BTIF_TRACE_EVENT("select timeout");
 continue;
 }
 else if (result < 0)
 {
            BTIF_TRACE_EVENT("select failed %s", strerror(errno));
 continue;
 }

        UIPC_LOCK();

 /* clear any wakeup interrupt */
        uipc_check_interrupt_locked();

 /* check pending task events */
        uipc_check_task_flags_locked();

 /* make sure we service audio channel first */
        uipc_check_fd_locked(UIPC_CH_ID_AV_AUDIO);

 /* check for other connections */
 for (ch_id = 0; ch_id < UIPC_CH_NUM; ch_id++)
 {
 if (ch_id != UIPC_CH_ID_AV_AUDIO)
                uipc_check_fd_locked(ch_id);
 }

        UIPC_UNLOCK();
 }

    BTIF_TRACE_EVENT("UIPC READ THREAD EXITING");

    uipc_main_cleanup();

    uipc_main.tid = 0;

    BTIF_TRACE_EVENT("UIPC READ THREAD DONE");
}
