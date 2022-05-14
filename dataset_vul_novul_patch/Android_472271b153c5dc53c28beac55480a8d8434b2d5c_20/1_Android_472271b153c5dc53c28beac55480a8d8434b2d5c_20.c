static inline int btif_hl_select_wake_reset(void){

     char sig_recv = 0;
 
     BTIF_TRACE_DEBUG("btif_hl_select_wake_reset");
    recv(signal_fds[0], &sig_recv, sizeof(sig_recv), MSG_WAITALL);
     return(int)sig_recv;
 }
