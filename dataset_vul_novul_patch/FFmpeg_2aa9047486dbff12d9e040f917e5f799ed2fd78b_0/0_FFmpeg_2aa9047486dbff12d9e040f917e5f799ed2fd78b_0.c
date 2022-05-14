static int mpeg4_decode_profile_level(MpegEncContext *s, GetBitContext *gb)
static int mpeg4_decode_profile_level(MpegEncContext *s, GetBitContext *gb, int *profile, int *level)
 {
 
    *profile = get_bits(gb, 4);
    *level   = get_bits(gb, 4);
 
    if (*profile == 0 && *level == 8) {
        *level = 0;
     }
 
     return 0;
}
