static int aac_sync(uint64_t state, AACAC3ParseContext *hdr_info,
        int *need_next_header, int *new_frame_start)
{
    GetBitContext bits;
    AACADTSHeaderInfo hdr;
     int size;
     union {
         uint64_t u64;
        uint8_t  u8[8 + FF_INPUT_BUFFER_PADDING_SIZE];
     } tmp;
 
     tmp.u64 = av_be2ne64(state);
    init_get_bits(&bits, tmp.u8+8-AAC_ADTS_HEADER_SIZE, AAC_ADTS_HEADER_SIZE * 8);

    if ((size = avpriv_aac_parse_header(&bits, &hdr)) < 0)
        return 0;
    *need_next_header = 0;
    *new_frame_start  = 1;
    hdr_info->sample_rate = hdr.sample_rate;
    hdr_info->channels    = ff_mpeg4audio_channels[hdr.chan_config];
    hdr_info->samples     = hdr.samples;
    hdr_info->bit_rate    = hdr.bit_rate;
    return size;
}
