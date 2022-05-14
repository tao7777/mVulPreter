 void red_channel_pipes_add_empty_msg(RedChannel *channel, int msg_type)
 {
    RingItem *link;
 
    RING_FOREACH(link, &channel->clients) {
         red_channel_client_pipe_add_empty_msg(
             SPICE_CONTAINEROF(link, RedChannelClient, channel_link),
             msg_type);
    }
}
