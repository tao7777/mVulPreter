static ssize_t read_and_process_frames(struct audio_stream_in *stream, void* buffer, ssize_t frames_num)
{
 struct stream_in *in = (struct stream_in *)stream;

     ssize_t frames_wr = 0; /* Number of frames actually read */
     size_t bytes_per_sample = audio_bytes_per_sample(stream->common.get_format(&stream->common));
     void *proc_buf_out = buffer;
#ifdef PREPROCESSING_ENABLED
    audio_buffer_t in_buf;
    audio_buffer_t out_buf;
    int i;
    bool has_processing = in->num_preprocessors != 0;
#endif
     /* Additional channels might be added on top of main_channels:
     * - aux_channels (by processing effects)
     * - extra channels due to HW limitations
    * In case of additional channels, we cannot work inplace
    */

     size_t src_channels = in->config.channels;
     size_t dst_channels = audio_channel_count_from_in_mask(in->main_channels);
     bool channel_remapping_needed = (dst_channels != src_channels);
    size_t src_buffer_size = frames_num * src_channels * bytes_per_sample;
 
 #ifdef PREPROCESSING_ENABLED
     if (has_processing) {
 /* since all the processing below is done in frames and using the config.channels
         * as the number of channels, no changes is required in case aux_channels are present */

         while (frames_wr < frames_num) {
             /* first reload enough frames at the end of process input buffer */
             if (in->proc_buf_frames < (size_t)frames_num) {
                ssize_t frames_rd;
                if (in->proc_buf_size < (size_t)frames_num) {
                    in->proc_buf_size = (size_t)frames_num;
                    in->proc_buf_in = realloc(in->proc_buf_in, src_buffer_size);
                    ALOG_ASSERT((in->proc_buf_in != NULL),
                                "process_frames() failed to reallocate proc_buf_in");
                    if (channel_remapping_needed) {
                        in->proc_buf_out = realloc(in->proc_buf_out, src_buffer_size);
                        ALOG_ASSERT((in->proc_buf_out != NULL),
                                    "process_frames() failed to reallocate proc_buf_out");
                        proc_buf_out = in->proc_buf_out;
                    }
                }
                frames_rd = read_frames(in,
                                        in->proc_buf_in +
                                            in->proc_buf_frames * src_channels * bytes_per_sample,
                                        frames_num - in->proc_buf_frames);
                  if (frames_rd < 0) {
                     /* Return error code */
                     frames_wr = frames_rd;
                     break;
 }
                in->proc_buf_frames += frames_rd;
 }

 
              /* in_buf.frameCount and out_buf.frameCount indicate respectively
               * the maximum number of frames to be consumed and produced by process() */
             in_buf.frameCount = in->proc_buf_frames;
            in_buf.s16 = in->proc_buf_in;
             out_buf.frameCount = frames_num - frames_wr;
            out_buf.s16 = (int16_t *)proc_buf_out + frames_wr * in->config.channels;
 
             /* FIXME: this works because of current pre processing library implementation that
              * does the actual process only when the last enabled effect process is called.
              * The generic solution is to have an output buffer for each effect and pass it as
              * input to the next.
              */
            for (i = 0; i < in->num_preprocessors; i++) {
                 (*in->preprocessors[i].effect_itfe)->process(in->preprocessors[i].effect_itfe,
                                                    &in_buf,
                                                    &out_buf);
 }

 /* process() has updated the number of frames consumed and produced in
             * in_buf.frameCount and out_buf.frameCount respectively
             * move remaining frames to the beginning of in->proc_buf_in */
            in->proc_buf_frames -= in_buf.frameCount;

 
             if (in->proc_buf_frames) {
                 memcpy(in->proc_buf_in,
                       in->proc_buf_in + in_buf.frameCount * src_channels * bytes_per_sample,
                       in->proc_buf_frames * in->config.channels * audio_bytes_per_sample(in_get_format(in)));
             }
 
             /* if not enough frames were passed to process(), read more and retry. */
 if (out_buf.frameCount == 0) {
                ALOGW("No frames produced by preproc");
 continue;
 }

 if ((frames_wr + (ssize_t)out_buf.frameCount) <= frames_num) {
                frames_wr += out_buf.frameCount;
 } else {
 /* The effect does not comply to the API. In theory, we should never end up here! */
                ALOGE("preprocessing produced too many frames: %d + %zd  > %d !",
 (unsigned int)frames_wr, out_buf.frameCount, (unsigned int)frames_num);
                frames_wr = frames_num;
 }
 }
 }
 else

 #endif //PREPROCESSING_ENABLED
     {
         /* No processing effects attached */
        if (channel_remapping_needed) {
            /* With additional channels, we cannot use original buffer */
            if (in->proc_buf_size < src_buffer_size) {
                in->proc_buf_size = src_buffer_size;
                in->proc_buf_out = realloc(in->proc_buf_out, src_buffer_size);
                ALOG_ASSERT((in->proc_buf_out != NULL),
                            "process_frames() failed to reallocate proc_buf_out");
            }
            proc_buf_out = in->proc_buf_out;
        }
         frames_wr = read_frames(in, proc_buf_out, frames_num);
         ALOG_ASSERT(frames_wr <= frames_num, "read more frames than requested");
     }
 
    if (channel_remapping_needed) {
         size_t ret = adjust_channels(proc_buf_out, src_channels, buffer, dst_channels,
            bytes_per_sample, frames_wr * src_channels * bytes_per_sample);
         ALOG_ASSERT(ret == (frames_wr * dst_channels * bytes_per_sample));
     }
 
 return frames_wr;
}
