int ParseCaffHeaderConfig (FILE *infile, char *infilename, char *fourcc, WavpackContext *wpc, WavpackConfig *config)
{
    uint32_t chan_chunk = 0, channel_layout = 0, bcount;
    unsigned char *channel_identities = NULL;
    unsigned char *channel_reorder = NULL;
    int64_t total_samples = 0, infilesize;
    CAFFileHeader caf_file_header;
    CAFChunkHeader caf_chunk_header;
    CAFAudioFormat caf_audio_format;
    int i;

    infilesize = DoGetFileSize (infile);
    memcpy (&caf_file_header, fourcc, 4);

    if ((!DoReadFile (infile, ((char *) &caf_file_header) + 4, sizeof (CAFFileHeader) - 4, &bcount) ||
        bcount != sizeof (CAFFileHeader) - 4)) {
            error_line ("%s is not a valid .CAF file!", infilename);
            return WAVPACK_SOFT_ERROR;
    }
    else if (!(config->qmode & QMODE_NO_STORE_WRAPPER) &&
        !WavpackAddWrapper (wpc, &caf_file_header, sizeof (CAFFileHeader))) {
            error_line ("%s", WavpackGetErrorMessage (wpc));
            return WAVPACK_SOFT_ERROR;
    }

    WavpackBigEndianToNative (&caf_file_header, CAFFileHeaderFormat);

    if (caf_file_header.mFileVersion != 1) {
        error_line ("%s: can't handle version %d .CAF files!", infilename, caf_file_header.mFileVersion);
        return WAVPACK_SOFT_ERROR;
    }


    while (1) {
        if (!DoReadFile (infile, &caf_chunk_header, sizeof (CAFChunkHeader), &bcount) ||
            bcount != sizeof (CAFChunkHeader)) {
                error_line ("%s is not a valid .CAF file!", infilename);
                return WAVPACK_SOFT_ERROR;
        }
        else if (!(config->qmode & QMODE_NO_STORE_WRAPPER) &&
            !WavpackAddWrapper (wpc, &caf_chunk_header, sizeof (CAFChunkHeader))) {
                error_line ("%s", WavpackGetErrorMessage (wpc));
                return WAVPACK_SOFT_ERROR;
        }

        WavpackBigEndianToNative (&caf_chunk_header, CAFChunkHeaderFormat);


        if (!strncmp (caf_chunk_header.mChunkType, "desc", 4)) {
            int supported = TRUE;

            if (caf_chunk_header.mChunkSize != sizeof (CAFAudioFormat) ||
                !DoReadFile (infile, &caf_audio_format, (uint32_t) caf_chunk_header.mChunkSize, &bcount) ||
                bcount != caf_chunk_header.mChunkSize) {
                    error_line ("%s is not a valid .CAF file!", infilename);
                    return WAVPACK_SOFT_ERROR;
            }
            else if (!(config->qmode & QMODE_NO_STORE_WRAPPER) &&
                !WavpackAddWrapper (wpc, &caf_audio_format, (uint32_t) caf_chunk_header.mChunkSize)) {
                    error_line ("%s", WavpackGetErrorMessage (wpc));
                    return WAVPACK_SOFT_ERROR;
            }

            WavpackBigEndianToNative (&caf_audio_format, CAFAudioFormatFormat);

            if (debug_logging_mode) {
                char formatstr [5];

                memcpy (formatstr, caf_audio_format.mFormatID, 4);
                formatstr [4] = 0;
                error_line ("format = %s, flags = %x, sampling rate = %g",
                    formatstr, caf_audio_format.mFormatFlags, caf_audio_format.mSampleRate);
                error_line ("packet = %d bytes and %d frames",
                    caf_audio_format.mBytesPerPacket, caf_audio_format.mFramesPerPacket);
                error_line ("channels per frame = %d, bits per channel = %d",
                    caf_audio_format.mChannelsPerFrame, caf_audio_format.mBitsPerChannel);
            }

            if (strncmp (caf_audio_format.mFormatID, "lpcm", 4) || (caf_audio_format.mFormatFlags & ~3))
                supported = FALSE;
            else if (caf_audio_format.mSampleRate < 1.0 || caf_audio_format.mSampleRate > 16777215.0 ||
                caf_audio_format.mSampleRate != floor (caf_audio_format.mSampleRate))
                    supported = FALSE;
            else if (!caf_audio_format.mChannelsPerFrame || caf_audio_format.mChannelsPerFrame > 256)
                supported = FALSE;
            else if (caf_audio_format.mBitsPerChannel < 1 || caf_audio_format.mBitsPerChannel > 32 ||
                ((caf_audio_format.mFormatFlags & CAF_FORMAT_FLOAT) && caf_audio_format.mBitsPerChannel != 32))
                    supported = FALSE;
            else if (caf_audio_format.mFramesPerPacket != 1 ||
                caf_audio_format.mBytesPerPacket / caf_audio_format.mChannelsPerFrame < (caf_audio_format.mBitsPerChannel + 7) / 8 ||
                caf_audio_format.mBytesPerPacket / caf_audio_format.mChannelsPerFrame > 4 ||
                caf_audio_format.mBytesPerPacket % caf_audio_format.mChannelsPerFrame)
                    supported = FALSE;

            if (!supported) {
                error_line ("%s is an unsupported .CAF format!", infilename);
                return WAVPACK_SOFT_ERROR;
            }

            config->bytes_per_sample = caf_audio_format.mBytesPerPacket / caf_audio_format.mChannelsPerFrame;
            config->float_norm_exp = (caf_audio_format.mFormatFlags & CAF_FORMAT_FLOAT) ? 127 : 0;
            config->bits_per_sample = caf_audio_format.mBitsPerChannel;
            config->num_channels = caf_audio_format.mChannelsPerFrame;
            config->sample_rate = (int) caf_audio_format.mSampleRate;

            if (!(caf_audio_format.mFormatFlags & CAF_FORMAT_LITTLE_ENDIAN) && config->bytes_per_sample > 1)
                config->qmode |= QMODE_BIG_ENDIAN;

            if (config->bytes_per_sample == 1)
                config->qmode |= QMODE_SIGNED_BYTES;

            if (debug_logging_mode) {
                if (config->float_norm_exp == 127)
                    error_line ("data format: 32-bit %s-endian floating point", (config->qmode & QMODE_BIG_ENDIAN) ? "big" : "little");
                else
                    error_line ("data format: %d-bit %s-endian integers stored in %d byte(s)",
                        config->bits_per_sample, (config->qmode & QMODE_BIG_ENDIAN) ? "big" : "little", config->bytes_per_sample);
             }
         }
         else if (!strncmp (caf_chunk_header.mChunkType, "chan", 4)) {
            CAFChannelLayout *caf_channel_layout;
 
            if (caf_chunk_header.mChunkSize < sizeof (CAFChannelLayout) || caf_chunk_header.mChunkSize > 1024) {
                error_line ("this .CAF file has an invalid 'chan' chunk!");
                return WAVPACK_SOFT_ERROR;
            }

            if (debug_logging_mode)
                error_line ("'chan' chunk is %d bytes", (int) caf_chunk_header.mChunkSize);

            caf_channel_layout = malloc ((size_t) caf_chunk_header.mChunkSize);

            if (!DoReadFile (infile, caf_channel_layout, (uint32_t) caf_chunk_header.mChunkSize, &bcount) ||
                 bcount != caf_chunk_header.mChunkSize) {
                     error_line ("%s is not a valid .CAF file!", infilename);
                     free (caf_channel_layout);
                    return WAVPACK_SOFT_ERROR;
            }
            else if (!(config->qmode & QMODE_NO_STORE_WRAPPER) &&
                !WavpackAddWrapper (wpc, caf_channel_layout, (uint32_t) caf_chunk_header.mChunkSize)) {
                    error_line ("%s", WavpackGetErrorMessage (wpc));
                    free (caf_channel_layout);
                    return WAVPACK_SOFT_ERROR;
            }

            WavpackBigEndianToNative (caf_channel_layout, CAFChannelLayoutFormat);
            chan_chunk = 1;

            if (config->channel_mask || (config->qmode & QMODE_CHANS_UNASSIGNED)) {
                error_line ("this CAF file already has channel order information!");
                free (caf_channel_layout);
                return WAVPACK_SOFT_ERROR;
            }

            switch (caf_channel_layout->mChannelLayoutTag) {
                case kCAFChannelLayoutTag_UseChannelDescriptions:
                    {
                        CAFChannelDescription *descriptions = (CAFChannelDescription *) (caf_channel_layout + 1);
                        int num_descriptions = caf_channel_layout->mNumberChannelDescriptions;
                        int label, cindex = 0, idents = 0;

                        if (caf_chunk_header.mChunkSize != sizeof (CAFChannelLayout) + sizeof (CAFChannelDescription) * num_descriptions ||
                            num_descriptions != config->num_channels) {
                                error_line ("channel descriptions in 'chan' chunk are the wrong size!");
                                free (caf_channel_layout);
                                return WAVPACK_SOFT_ERROR;
                        }

                        if (num_descriptions >= 256) {
                            error_line ("%d channel descriptions is more than we can handle...ignoring!");
                            break;
                        }


                        channel_reorder = malloc (num_descriptions);
                        memset (channel_reorder, -1, num_descriptions);
                        channel_identities = malloc (num_descriptions+1);


                        for (i = 0; i < num_descriptions; ++i) {
                            WavpackBigEndianToNative (descriptions + i, CAFChannelDescriptionFormat);

                            if (debug_logging_mode)
                                error_line ("chan %d --> %d", i + 1, descriptions [i].mChannelLabel);
                        }


                        for (label = 1; label <= 18; ++label)
                            for (i = 0; i < num_descriptions; ++i)
                                if (descriptions [i].mChannelLabel == label) {
                                    config->channel_mask |= 1 << (label - 1);
                                    channel_reorder [i] = cindex++;
                                    break;
                                }


                        for (i = 0; i < num_descriptions; ++i)
                            if (channel_reorder [i] == (unsigned char) -1) {
                                uint32_t clabel = descriptions [i].mChannelLabel;

                                if (clabel == 0 || clabel == 0xffffffff || clabel == 100)
                                    channel_identities [idents++] = 0xff;
                                else if ((clabel >= 33 && clabel <= 44) || (clabel >= 200 && clabel <= 207) || (clabel >= 301 && clabel <= 305))
                                    channel_identities [idents++] = clabel >= 301 ? clabel - 80 : clabel;
                                else {
                                    error_line ("warning: unknown channel descriptions label: %d", clabel);
                                    channel_identities [idents++] = 0xff;
                                }

                                channel_reorder [i] = cindex++;
                            }


                        for (i = 0; i < num_descriptions; ++i)
                            if (channel_reorder [i] != i)
                                break;

                        if (i == num_descriptions) {
                            free (channel_reorder);                 // no reordering required, so don't
                            channel_reorder = NULL;
                        }
                        else {
                            config->qmode |= QMODE_REORDERED_CHANS; // reordering required, put channel count into layout
                            channel_layout = num_descriptions;
                        }

                        if (!idents) {                              // if no non-MS channels, free the identities string
                            free (channel_identities);
                            channel_identities = NULL;
                        }
                        else
                            channel_identities [idents] = 0;        // otherwise NULL terminate it

                        if (debug_logging_mode) {
                            error_line ("layout_tag = 0x%08x, so generated bitmap of 0x%08x from %d descriptions, %d non-MS",
                                caf_channel_layout->mChannelLayoutTag, config->channel_mask,
                                caf_channel_layout->mNumberChannelDescriptions, idents);


                            if (channel_reorder && num_descriptions <= 8) {
                                char reorder_string [] = "12345678";

                                for (i = 0; i < num_descriptions; ++i)
                                    reorder_string [i] = channel_reorder [i] + '1';

                                reorder_string [i] = 0;
                                error_line ("reordering string = \"%s\"\n", reorder_string);
                            }
                        }
                    }

                    break;

                case kCAFChannelLayoutTag_UseChannelBitmap:
                    config->channel_mask = caf_channel_layout->mChannelBitmap;

                    if (debug_logging_mode)
                        error_line ("layout_tag = 0x%08x, so using supplied bitmap of 0x%08x",
                            caf_channel_layout->mChannelLayoutTag, caf_channel_layout->mChannelBitmap);

                    break;

                default:
                    for (i = 0; i < NUM_LAYOUTS; ++i)
                        if (caf_channel_layout->mChannelLayoutTag == layouts [i].mChannelLayoutTag) {
                            config->channel_mask = layouts [i].mChannelBitmap;
                            channel_layout = layouts [i].mChannelLayoutTag;

                            if (layouts [i].mChannelReorder) {
                                channel_reorder = (unsigned char *) strdup (layouts [i].mChannelReorder);
                                config->qmode |= QMODE_REORDERED_CHANS;
                            }

                            if (layouts [i].mChannelIdentities)
                                channel_identities = (unsigned char *) strdup (layouts [i].mChannelIdentities);

                            if (debug_logging_mode)
                                error_line ("layout_tag 0x%08x found in table, bitmap = 0x%08x, reorder = %s, identities = %s",
                                    channel_layout, config->channel_mask, channel_reorder ? "yes" : "no", channel_identities ? "yes" : "no");

                            break;
                        }

                    if (i == NUM_LAYOUTS && debug_logging_mode)
                        error_line ("layout_tag 0x%08x not found in table...all channels unassigned",
                            caf_channel_layout->mChannelLayoutTag);

                    break;
            }

            free (caf_channel_layout);
        }
        else if (!strncmp (caf_chunk_header.mChunkType, "data", 4)) {     // on the data chunk, get size and exit loop
            uint32_t mEditCount;

            if (!DoReadFile (infile, &mEditCount, sizeof (mEditCount), &bcount) ||
                bcount != sizeof (mEditCount)) {
                    error_line ("%s is not a valid .CAF file!", infilename);
                    return WAVPACK_SOFT_ERROR;
            }
            else if (!(config->qmode & QMODE_NO_STORE_WRAPPER) &&
                !WavpackAddWrapper (wpc, &mEditCount, sizeof (mEditCount))) {
                    error_line ("%s", WavpackGetErrorMessage (wpc));
                    return WAVPACK_SOFT_ERROR;
            }

            if ((config->qmode & QMODE_IGNORE_LENGTH) || caf_chunk_header.mChunkSize == -1) {
                config->qmode |= QMODE_IGNORE_LENGTH;

                if (infilesize && DoGetFilePosition (infile) != -1)
                    total_samples = (infilesize - DoGetFilePosition (infile)) / caf_audio_format.mBytesPerPacket;
                else
                    total_samples = -1;
            }
            else {
                if (infilesize && infilesize - caf_chunk_header.mChunkSize > 16777216) {
                    error_line (".CAF file %s has over 16 MB of extra CAFF data, probably is corrupt!", infilename);
                    return WAVPACK_SOFT_ERROR;
                }

                if ((caf_chunk_header.mChunkSize - 4) % caf_audio_format.mBytesPerPacket) {
                    error_line (".CAF file %s has an invalid data chunk size, probably is corrupt!", infilename);
                    return WAVPACK_SOFT_ERROR;
                }

                total_samples = (caf_chunk_header.mChunkSize - 4) / caf_audio_format.mBytesPerPacket;

                if (!total_samples) {
                    error_line ("this .CAF file has no audio samples, probably is corrupt!");
                    return WAVPACK_SOFT_ERROR;
                }

                if (total_samples > MAX_WAVPACK_SAMPLES) {
                    error_line ("%s has too many samples for WavPack!", infilename);
                    return WAVPACK_SOFT_ERROR;
                }
            }

            break;
         }
         else {          // just copy unknown chunks to output file
 
            uint32_t bytes_to_copy = (uint32_t) caf_chunk_header.mChunkSize;
            char *buff;

            if (caf_chunk_header.mChunkSize < 0 || caf_chunk_header.mChunkSize > 1048576) {
                error_line ("%s is not a valid .CAF file!", infilename);
                return WAVPACK_SOFT_ERROR;
            }

            buff = malloc (bytes_to_copy);
 
             if (debug_logging_mode)
                 error_line ("extra unknown chunk \"%c%c%c%c\" of %d bytes",
                    caf_chunk_header.mChunkType [0], caf_chunk_header.mChunkType [1], caf_chunk_header.mChunkType [2],
                    caf_chunk_header.mChunkType [3], caf_chunk_header.mChunkSize);

            if (!DoReadFile (infile, buff, bytes_to_copy, &bcount) ||
                bcount != bytes_to_copy ||
                (!(config->qmode & QMODE_NO_STORE_WRAPPER) &&
                !WavpackAddWrapper (wpc, buff, bytes_to_copy))) {
                    error_line ("%s", WavpackGetErrorMessage (wpc));
                    free (buff);
                    return WAVPACK_SOFT_ERROR;
            }

            free (buff);
        }
    }

    if (!chan_chunk && !config->channel_mask && config->num_channels <= 2 && !(config->qmode & QMODE_CHANS_UNASSIGNED))
        config->channel_mask = 0x5 - config->num_channels;

    if (!WavpackSetConfiguration64 (wpc, config, total_samples, channel_identities)) {
        error_line ("%s", WavpackGetErrorMessage (wpc));
        return WAVPACK_SOFT_ERROR;
    }

    if (channel_identities)
        free (channel_identities);

    if (channel_layout || channel_reorder) {
        if (!WavpackSetChannelLayout (wpc, channel_layout, channel_reorder)) {
            error_line ("problem with setting channel layout (should not happen)");
            return WAVPACK_SOFT_ERROR;
        }

        if (channel_reorder)
            free (channel_reorder);
    }

    return WAVPACK_NO_ERROR;
}
