int WavpackSetConfiguration64 (WavpackContext *wpc, WavpackConfig *config, int64_t total_samples, const unsigned char *chan_ids)
{
    uint32_t flags, bps = 0;
    uint32_t chan_mask = config->channel_mask;
     int num_chans = config->num_channels;
     int i;
 
    if (!config->sample_rate) {
        strcpy (wpc->error_message, "sample rate cannot be zero!");
        return FALSE;
    }

     wpc->stream_version = (config->flags & CONFIG_COMPATIBLE_WRITE) ? CUR_STREAM_VERS : MAX_STREAM_VERS;
 
     if ((config->qmode & QMODE_DSD_AUDIO) && config->bytes_per_sample == 1 && config->bits_per_sample == 8) {
#ifdef ENABLE_DSD
        wpc->dsd_multiplier = 1;
        flags = DSD_FLAG;

        for (i = 14; i >= 0; --i)
            if (config->sample_rate % sample_rates [i] == 0) {
                int divisor = config->sample_rate / sample_rates [i];

                if (divisor && (divisor & (divisor - 1)) == 0) {
                    config->sample_rate /= divisor;
                    wpc->dsd_multiplier = divisor;
                    break;
                }
            }

        if (config->flags & CONFIG_HYBRID_FLAG) {
            strcpy (wpc->error_message, "hybrid mode not available for DSD!");
            return FALSE;
        }

        config->flags &= (CONFIG_HIGH_FLAG | CONFIG_MD5_CHECKSUM | CONFIG_PAIR_UNDEF_CHANS);
        config->float_norm_exp = config->xmode = 0;
#else
        strcpy (wpc->error_message, "libwavpack not configured for DSD!");
        return FALSE;
#endif
    }
    else
        flags = config->bytes_per_sample - 1;

    wpc->total_samples = total_samples;
    wpc->config.sample_rate = config->sample_rate;
    wpc->config.num_channels = config->num_channels;
    wpc->config.channel_mask = config->channel_mask;
    wpc->config.bits_per_sample = config->bits_per_sample;
    wpc->config.bytes_per_sample = config->bytes_per_sample;
    wpc->config.block_samples = config->block_samples;
    wpc->config.flags = config->flags;
    wpc->config.qmode = config->qmode;

    if (config->flags & CONFIG_VERY_HIGH_FLAG)
        wpc->config.flags |= CONFIG_HIGH_FLAG;

    for (i = 0; i < 15; ++i)
        if (wpc->config.sample_rate == sample_rates [i])
            break;

    flags |= i << SRATE_LSB;


    if (!(flags & DSD_FLAG)) {
        if (config->float_norm_exp) {
            wpc->config.float_norm_exp = config->float_norm_exp;
            wpc->config.flags |= CONFIG_FLOAT_DATA;
            flags |= FLOAT_DATA;
        }
        else
            flags |= ((config->bytes_per_sample * 8) - config->bits_per_sample) << SHIFT_LSB;

        if (config->flags & CONFIG_HYBRID_FLAG) {
            flags |= HYBRID_FLAG | HYBRID_BITRATE | HYBRID_BALANCE;

            if (!(wpc->config.flags & CONFIG_SHAPE_OVERRIDE)) {
                wpc->config.flags |= CONFIG_HYBRID_SHAPE | CONFIG_AUTO_SHAPING;
                flags |= HYBRID_SHAPE | NEW_SHAPING;
            }
            else if (wpc->config.flags & CONFIG_HYBRID_SHAPE) {
                wpc->config.shaping_weight = config->shaping_weight;
                flags |= HYBRID_SHAPE | NEW_SHAPING;
            }

            if (wpc->config.flags & (CONFIG_CROSS_DECORR | CONFIG_OPTIMIZE_WVC))
                flags |= CROSS_DECORR;

            if (config->flags & CONFIG_BITRATE_KBPS) {
                bps = (uint32_t) floor (config->bitrate * 256000.0 / config->sample_rate / config->num_channels + 0.5);

                if (bps > (64 << 8))
                    bps = 64 << 8;
            }
            else
                bps = (uint32_t) floor (config->bitrate * 256.0 + 0.5);
        }
        else
            flags |= CROSS_DECORR;

        if (!(config->flags & CONFIG_JOINT_OVERRIDE) || (config->flags & CONFIG_JOINT_STEREO))
            flags |= JOINT_STEREO;

        if (config->flags & CONFIG_CREATE_WVC)
            wpc->wvc_flag = TRUE;
    }


    if (chan_ids) {
        int lastchan = 0, mask_copy = chan_mask;

        if ((int) strlen ((char *) chan_ids) > num_chans) {          // can't be more than num channels!
            strcpy (wpc->error_message, "chan_ids longer than num channels!");
            return FALSE;
        }


        while (*chan_ids)
            if (*chan_ids <= 32 && *chan_ids > lastchan && (mask_copy & (1 << (*chan_ids-1)))) {
                mask_copy &= ~(1 << (*chan_ids-1));
                lastchan = *chan_ids++;
            }
            else
                break;


        for (i = 0; chan_ids [i]; i++)
            if (chan_ids [i] != 0xff) {
                wpc->channel_identities = (unsigned char *) strdup ((char *) chan_ids);
                break;
            }
    }


    for (wpc->current_stream = 0; num_chans; wpc->current_stream++) {
        WavpackStream *wps = malloc (sizeof (WavpackStream));
        unsigned char left_chan_id = 0, right_chan_id = 0;
        int pos, chans = 1;

        wpc->streams = realloc (wpc->streams, (wpc->current_stream + 1) * sizeof (wpc->streams [0]));
        wpc->streams [wpc->current_stream] = wps;
        CLEAR (*wps);

        if (chan_mask)
            for (pos = 0; pos < 32; ++pos)
                if (chan_mask & (1 << pos)) {
                    if (left_chan_id) {
                        right_chan_id = pos + 1;
                        break;
                    }
                    else {
                        chan_mask &= ~(1 << pos);
                        left_chan_id = pos + 1;
                    }
                }

        while (!right_chan_id && chan_ids && *chan_ids)
            if (left_chan_id)
                right_chan_id = *chan_ids;
            else
                left_chan_id = *chan_ids++;

        if (!left_chan_id)
            left_chan_id = right_chan_id = 0xff;
        else if (!right_chan_id)
            right_chan_id = 0xff;

        if (num_chans >= 2) {
            if ((config->flags & CONFIG_PAIR_UNDEF_CHANS) && left_chan_id == 0xff && right_chan_id == 0xff)
                chans = 2;
            else
                for (i = 0; i < NUM_STEREO_PAIRS; ++i)
                    if ((left_chan_id == stereo_pairs [i].a && right_chan_id == stereo_pairs [i].b) ||
                        (left_chan_id == stereo_pairs [i].b && right_chan_id == stereo_pairs [i].a)) {
                            if (right_chan_id <= 32 && (chan_mask & (1 << (right_chan_id-1))))
                                chan_mask &= ~(1 << (right_chan_id-1));
                            else if (chan_ids && *chan_ids == right_chan_id)
                                chan_ids++;

                            chans = 2;
                            break;
                        }
        }

        num_chans -= chans;

        if (num_chans && wpc->current_stream == NEW_MAX_STREAMS - 1)
            break;

        memcpy (wps->wphdr.ckID, "wvpk", 4);
        wps->wphdr.ckSize = sizeof (WavpackHeader) - 8;
        SET_TOTAL_SAMPLES (wps->wphdr, wpc->total_samples);
        wps->wphdr.version = wpc->stream_version;
        wps->wphdr.flags = flags;
        wps->bits = bps;

        if (!wpc->current_stream)
            wps->wphdr.flags |= INITIAL_BLOCK;

        if (!num_chans)
            wps->wphdr.flags |= FINAL_BLOCK;

        if (chans == 1) {
            wps->wphdr.flags &= ~(JOINT_STEREO | CROSS_DECORR | HYBRID_BALANCE);
            wps->wphdr.flags |= MONO_FLAG;
        }
    }

    wpc->num_streams = wpc->current_stream;
    wpc->current_stream = 0;

    if (num_chans) {
        strcpy (wpc->error_message, "too many channels!");
        return FALSE;
    }

    if (config->flags & CONFIG_EXTRA_MODE)
        wpc->config.xmode = config->xmode ? config->xmode : 1;

    return TRUE;
}
