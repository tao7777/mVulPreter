static void uipc_check_interrupt_locked(void)
{
 if (SAFE_FD_ISSET(uipc_main.signal_fds[0], &uipc_main.read_set))

     {
         char sig_recv = 0;
        TEMP_FAILURE_RETRY(recv(uipc_main.signal_fds[0], &sig_recv, sizeof(sig_recv), MSG_WAITALL));
     }
 }
