static ssize_t in_read(struct audio_stream_in *stream, void* buffer,
 size_t bytes)
{
 struct a2dp_stream_in *in = (struct a2dp_stream_in *)stream;
 int read;

    DEBUG("read %zu bytes, state: %d", bytes, in->common.state);

 if (in->common.state == AUDIO_A2DP_STATE_SUSPENDED)
 {
        DEBUG("stream suspended");
 return -1;
 }

 /* only allow autostarting if we are in stopped or standby */
 if ((in->common.state == AUDIO_A2DP_STATE_STOPPED) ||
 (in->common.state == AUDIO_A2DP_STATE_STANDBY))
 {
        pthread_mutex_lock(&in->common.lock);

 if (start_audio_datapath(&in->common) < 0)
 {
 /* emulate time this write represents to avoid very fast write
               failures during transition periods or remote suspend */

 int us_delay = calc_audiotime(in->common.cfg, bytes);

 
             DEBUG("emulate a2dp read delay (%d us)", us_delay);
 
            TEMP_FAILURE_RETRY(usleep(us_delay));
             pthread_mutex_unlock(&in->common.lock);
             return -1;
         }

        pthread_mutex_unlock(&in->common.lock);
 }
 else if (in->common.state != AUDIO_A2DP_STATE_STARTED)
 {
        ERROR("stream not in stopped or standby");
 return -1;
 }

    read = skt_read(in->common.audio_fd, buffer, bytes);

 if (read == -1)
 {
        skt_disconnect(in->common.audio_fd);
        in->common.audio_fd = AUDIO_SKT_DISCONNECTED;
        in->common.state = AUDIO_A2DP_STATE_STOPPED;
 } else if (read == 0) {
        DEBUG("read time out - return zeros");
        memset(buffer, 0, bytes);
        read = bytes;
 }

    DEBUG("read %d bytes out of %zu bytes", read, bytes);
 return read;
}
