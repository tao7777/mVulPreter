static void a2dp_open_ctrl_path(struct a2dp_stream_common *common)
{
 int i;

 /* retry logic to catch any timing variations on control channel */
 for (i = 0; i < CTRL_CHAN_RETRY_COUNT; i++)
 {
 /* connect control channel if not already connected */
 if ((common->ctrl_fd = skt_connect(A2DP_CTRL_PATH, common->buffer_sz)) > 0)
 {
 /* success, now check if stack is ready */
 if (check_a2dp_ready(common) == 0)

                 break;
 
             ERROR("error : a2dp not ready, wait 250 ms and retry");
            usleep(250000);
             skt_disconnect(common->ctrl_fd);
             common->ctrl_fd = AUDIO_SKT_DISCONNECTED;
         }
 
         /* ctrl channel not ready, wait a bit */
        usleep(250000);
     }
 }
