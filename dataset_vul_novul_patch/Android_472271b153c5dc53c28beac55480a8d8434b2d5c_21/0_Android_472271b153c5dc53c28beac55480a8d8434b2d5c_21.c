 static inline int btif_hl_select_wakeup(void){
     char sig_on = btif_hl_signal_select_wakeup;
     BTIF_TRACE_DEBUG("btif_hl_select_wakeup");
    return TEMP_FAILURE_RETRY(send(signal_fds[1], &sig_on, sizeof(sig_on), 0));
 }
