static void *btif_hl_select_thread(void *arg){
    fd_set org_set, curr_set;
 int r, max_curr_s, max_org_s;
    UNUSED(arg);

    BTIF_TRACE_DEBUG("entered btif_hl_select_thread");
    FD_ZERO(&org_set);
    max_org_s = btif_hl_select_wakeup_init(&org_set);
    BTIF_TRACE_DEBUG("max_s=%d ", max_org_s);

 for (;;)
 {
        r = 0;

         BTIF_TRACE_DEBUG("set curr_set = org_set ");
         curr_set = org_set;
         max_curr_s = max_org_s;
        int ret = select((max_curr_s + 1), &curr_set, NULL, NULL, NULL);
         BTIF_TRACE_DEBUG("select unblocked ret=%d", ret);
         if (ret == -1)
         {
            BTIF_TRACE_DEBUG("select() ret -1, exit the thread");
            btif_hl_thread_cleanup();
            select_thread_id = -1;
 return 0;
 }
 else if (ret)
 {
            BTIF_TRACE_DEBUG("btif_hl_select_wake_signaled, signal ret=%d", ret);
 if (btif_hl_select_wake_signaled(&curr_set))
 {
                r = btif_hl_select_wake_reset();
                BTIF_TRACE_DEBUG("btif_hl_select_wake_signaled, signal:%d", r);
 if (r == btif_hl_signal_select_wakeup || r == btif_hl_signal_select_close_connected )
 {
                    btif_hl_select_wakeup_callback(&org_set, r);
 }
 else if( r == btif_hl_signal_select_exit)
 {
                    btif_hl_thread_cleanup();
                    BTIF_TRACE_DEBUG("Exit hl_select_thread for btif_hl_signal_select_exit");
 return 0;
 }
 }

            btif_hl_select_monitor_callback(&curr_set, &org_set);
            max_org_s = btif_hl_update_maxfd(max_org_s);
 }
 else
            BTIF_TRACE_DEBUG("no data, select ret: %d\n", ret);
 }
    BTIF_TRACE_DEBUG("leaving hl_select_thread");
 return 0;
}
