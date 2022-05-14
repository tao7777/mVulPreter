static int a2dp_command(struct a2dp_stream_common *common, char cmd)
{
 char ack;


     DEBUG("A2DP COMMAND %s", dump_a2dp_ctrl_event(cmd));
 
     /* send command */
    if (TEMP_FAILURE_RETRY(send(common->ctrl_fd, &cmd, 1, MSG_NOSIGNAL)) == -1)
     {
         ERROR("cmd failed (%s)", strerror(errno));
         skt_disconnect(common->ctrl_fd);
        common->ctrl_fd = AUDIO_SKT_DISCONNECTED;
 return -1;
 }

 /* wait for ack byte */
 if (a2dp_ctrl_receive(common, &ack, 1) < 0)
 return -1;

    DEBUG("A2DP COMMAND %s DONE STATUS %d", dump_a2dp_ctrl_event(cmd), ack);

 if (ack == A2DP_CTRL_ACK_INCALL_FAILURE)
 return ack;
 if (ack != A2DP_CTRL_ACK_SUCCESS)
 return -1;

 return 0;
}
