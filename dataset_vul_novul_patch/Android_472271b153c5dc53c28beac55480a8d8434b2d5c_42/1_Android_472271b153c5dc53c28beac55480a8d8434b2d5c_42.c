static int process_cmd_sock(int h)

 {
     sock_cmd_t cmd = {-1, 0, 0, 0, 0};
     int fd = ts[h].cmd_fdr;
    if(recv(fd, &cmd, sizeof(cmd), MSG_WAITALL) != sizeof(cmd))
     {
         APPL_TRACE_ERROR("recv cmd errno:%d", errno);
         return FALSE;
 }
    APPL_TRACE_DEBUG("cmd.id:%d", cmd.id);
 switch(cmd.id)
 {
 case CMD_ADD_FD:
            add_poll(h, cmd.fd, cmd.type, cmd.flags, cmd.user_id);
 break;
 case CMD_REMOVE_FD:
 for (int i = 1; i < MAX_POLL; ++i)
 {
 poll_slot_t *poll_slot = &ts[h].ps[i];
 if (poll_slot->pfd.fd == cmd.fd)
 {
                    remove_poll(h, poll_slot, poll_slot->flags);
 break;
 }
 }
            close(cmd.fd);
 break;
 case CMD_WAKEUP:
 break;
 case CMD_USER_PRIVATE:
            asrt(ts[h].cmd_callback);
 if(ts[h].cmd_callback)
                ts[h].cmd_callback(fd, cmd.type, cmd.flags, cmd.user_id);
 break;
 case CMD_EXIT:
 return FALSE;
 default:
            APPL_TRACE_DEBUG("unknown cmd: %d", cmd.id);
 break;
 }
 return TRUE;
}
