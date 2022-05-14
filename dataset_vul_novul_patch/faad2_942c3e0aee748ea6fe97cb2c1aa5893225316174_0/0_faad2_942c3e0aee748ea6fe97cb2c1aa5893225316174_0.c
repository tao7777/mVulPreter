static uint8_t excluded_channels(bitfile *ld, drc_info *drc)
{
    uint8_t i, n = 0;
    uint8_t num_excl_chan = 7;

    for (i = 0; i < 7; i++)
    {
        drc->exclude_mask[i] = faad_get1bit(ld
            DEBUGVAR(1,103,"excluded_channels(): exclude_mask"));
    }
    n++;

     while ((drc->additional_excluded_chns[n-1] = faad_get1bit(ld
         DEBUGVAR(1,104,"excluded_channels(): additional_excluded_chns"))) == 1)
     {
        if (i >= MAX_CHANNELS - num_excl_chan - 7)
            return n;
         for (i = num_excl_chan; i < num_excl_chan+7; i++)
         {
             drc->exclude_mask[i] = faad_get1bit(ld
                DEBUGVAR(1,105,"excluded_channels(): exclude_mask"));
        }
        n++;
        num_excl_chan += 7;
    }

    return n;
}
