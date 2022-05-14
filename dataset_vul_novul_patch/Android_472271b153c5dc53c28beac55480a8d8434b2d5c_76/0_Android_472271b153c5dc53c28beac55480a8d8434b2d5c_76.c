static inline void uipc_wakeup_locked(void)

 {
     char sig_on = 1;
     BTIF_TRACE_EVENT("UIPC SEND WAKE UP");
    TEMP_FAILURE_RETRY(send(uipc_main.signal_fds[1], &sig_on, sizeof(sig_on), 0));
 }
