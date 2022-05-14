 void red_channel_pipes_add_type(RedChannel *channel, int pipe_item_type)
 {
    RingItem *link;
 
    RING_FOREACH(link, &channel->clients) {
         red_channel_client_pipe_add_type(
             SPICE_CONTAINEROF(link, RedChannelClient, channel_link),
             pipe_item_type);
    }
}
