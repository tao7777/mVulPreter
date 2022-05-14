 static inline int btif_hl_select_close_connected(void){
     char sig_on = btif_hl_signal_select_close_connected;
     BTIF_TRACE_DEBUG("btif_hl_select_close_connected");
    return send(signal_fds[1], &sig_on, sizeof(sig_on), 0);
 }
