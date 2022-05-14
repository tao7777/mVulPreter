static int adev_open_output_stream(struct audio_hw_device *dev,
 audio_io_handle_t handle,
 audio_devices_t devices,
 audio_output_flags_t flags,
 struct audio_config *config,
 struct audio_stream_out **stream_out,
 const char *address)

{
 struct a2dp_audio_device *a2dp_dev = (struct a2dp_audio_device *)dev;
 struct a2dp_stream_out *out;
 int ret = 0;
 int i;
    UNUSED(address);
    UNUSED(handle);
    UNUSED(devices);
    UNUSED(flags);

    INFO("opening output");

    out = (struct a2dp_stream_out *)calloc(1, sizeof(struct a2dp_stream_out));

 if (!out)
 return -ENOMEM;

    out->stream.common.get_sample_rate = out_get_sample_rate;
    out->stream.common.set_sample_rate = out_set_sample_rate;
    out->stream.common.get_buffer_size = out_get_buffer_size;
    out->stream.common.get_channels = out_get_channels;
    out->stream.common.get_format = out_get_format;
    out->stream.common.set_format = out_set_format;
    out->stream.common.standby = out_standby;
    out->stream.common.dump = out_dump;
    out->stream.common.set_parameters = out_set_parameters;
    out->stream.common.get_parameters = out_get_parameters;
    out->stream.common.add_audio_effect = out_add_audio_effect;
    out->stream.common.remove_audio_effect = out_remove_audio_effect;
    out->stream.get_latency = out_get_latency;
    out->stream.set_volume = out_set_volume;
    out->stream.write = out_write;
    out->stream.get_render_position = out_get_render_position;
    out->stream.get_presentation_position = out_get_presentation_position;


 /* initialize a2dp specifics */
    a2dp_stream_common_init(&out->common);

    out->common.cfg.channel_flags = AUDIO_STREAM_DEFAULT_CHANNEL_FLAG;
    out->common.cfg.format = AUDIO_STREAM_DEFAULT_FORMAT;
    out->common.cfg.rate = AUDIO_STREAM_DEFAULT_RATE;

 /* set output config values */
 if (config)
 {
      config->format = out_get_format((const struct audio_stream *)&out->stream);
      config->sample_rate = out_get_sample_rate((const struct audio_stream *)&out->stream);
      config->channel_mask = out_get_channels((const struct audio_stream *)&out->stream);
 }
 *stream_out = &out->stream;
    a2dp_dev->output = out;

    a2dp_open_ctrl_path(&out->common);
 if (out->common.ctrl_fd == AUDIO_SKT_DISCONNECTED)
 {
        ERROR("ctrl socket failed to connect (%s)", strerror(errno));
        ret = -1;
 goto err_open;
 }


     DEBUG("success");
     /* Delay to ensure Headset is in proper state when START is initiated
        from DUT immediately after the connection due to ongoing music playback. */
    usleep(250000);
     return 0;
 
 err_open:
    free(out);
 *stream_out = NULL;
    a2dp_dev->output = NULL;
    ERROR("failed");
 return ret;
}
