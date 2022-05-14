 static int a2dp_ctrl_receive(struct a2dp_stream_common *common, void* buffer, int length)
 {
    int ret = recv(common->ctrl_fd, buffer, length, MSG_NOSIGNAL);
     if (ret < 0)
     {
         ERROR("ack failed (%s)", strerror(errno));
         if (errno == EINTR)
         {
             /* retry again */
            ret = recv(common->ctrl_fd, buffer, length, MSG_NOSIGNAL);
             if (ret < 0)
             {
                ERROR("ack failed (%s)", strerror(errno));
               skt_disconnect(common->ctrl_fd);
               common->ctrl_fd = AUDIO_SKT_DISCONNECTED;
 return -1;
 }
 }
 else
 {
               skt_disconnect(common->ctrl_fd);
               common->ctrl_fd = AUDIO_SKT_DISCONNECTED;
 return -1;

 }
 }
 return ret;
}
