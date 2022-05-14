static ssize_t out_write(struct audio_stream_out *stream, const void* buffer,
 size_t bytes)
{
 struct a2dp_stream_out *out = (struct a2dp_stream_out *)stream;
 int sent;

    DEBUG("write %zu bytes (fd %d)", bytes, out->common.audio_fd);

    pthread_mutex_lock(&out->common.lock);

 if (out->common.state == AUDIO_A2DP_STATE_SUSPENDED)
 {
        DEBUG("stream suspended");
        pthread_mutex_unlock(&out->common.lock);
 return -1;
 }

 /* only allow autostarting if we are in stopped or standby */
 if ((out->common.state == AUDIO_A2DP_STATE_STOPPED) ||
 (out->common.state == AUDIO_A2DP_STATE_STANDBY))
 {
 if (start_audio_datapath(&out->common) < 0)
 {
 /* emulate time this write represents to avoid very fast write
               failures during transition periods or remote suspend */

 int us_delay = calc_audiotime(out->common.cfg, bytes);

 
             DEBUG("emulate a2dp write delay (%d us)", us_delay);
 
            usleep(us_delay);
             pthread_mutex_unlock(&out->common.lock);
             return -1;
         }
 }
 else if (out->common.state != AUDIO_A2DP_STATE_STARTED)
 {
        ERROR("stream not in stopped or standby");
        pthread_mutex_unlock(&out->common.lock);
 return -1;
 }

    pthread_mutex_unlock(&out->common.lock);
    sent = skt_write(out->common.audio_fd, buffer,  bytes);

 if (sent == -1) {
        skt_disconnect(out->common.audio_fd);
        out->common.audio_fd = AUDIO_SKT_DISCONNECTED;
 if (out->common.state != AUDIO_A2DP_STATE_SUSPENDED)
            out->common.state = AUDIO_A2DP_STATE_STOPPED;
 else
            ERROR("write failed : stream suspended, avoid resetting state");
 } else {
 const size_t frames = bytes / audio_stream_out_frame_size(stream);
        out->frames_rendered += frames;
        out->frames_presented += frames;
 }

    DEBUG("wrote %d bytes out of %zu bytes", sent, bytes);
 return sent;
}
