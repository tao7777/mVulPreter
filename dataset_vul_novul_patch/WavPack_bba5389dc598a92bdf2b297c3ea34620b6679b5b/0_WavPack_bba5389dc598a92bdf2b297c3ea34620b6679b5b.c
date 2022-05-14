int WavpackVerifySingleBlock (unsigned char *buffer, int verify_checksum)
{
    WavpackHeader *wphdr = (WavpackHeader *) buffer;
    uint32_t checksum_passed = 0, bcount, meta_bc;
    unsigned char *dp, meta_id, c1, c2;

    if (strncmp (wphdr->ckID, "wvpk", 4) || wphdr->ckSize + 8 < sizeof (WavpackHeader))
        return FALSE;

    bcount = wphdr->ckSize - sizeof (WavpackHeader) + 8;
    dp = (unsigned char *)(wphdr + 1);

    while (bcount >= 2) {
        meta_id = *dp++;
        c1 = *dp++;

        meta_bc = c1 << 1;
        bcount -= 2;

        if (meta_id & ID_LARGE) {
            if (bcount < 2)
                return FALSE;

            c1 = *dp++;
            c2 = *dp++;
            meta_bc += ((uint32_t) c1 << 9) + ((uint32_t) c2 << 17);
            bcount -= 2;
        }

        if (bcount < meta_bc)
            return FALSE;

        if (verify_checksum && (meta_id & ID_UNIQUE) == ID_BLOCK_CHECKSUM) {
#ifdef BITSTREAM_SHORTS
            uint16_t *csptr = (uint16_t*) buffer;
#else
            unsigned char *csptr = buffer;
#endif
            int wcount = (int)(dp - 2 - buffer) >> 1;
            uint32_t csum = (uint32_t) -1;

            if ((meta_id & ID_ODD_SIZE) || meta_bc < 2 || meta_bc > 4)
                return FALSE;

#ifdef BITSTREAM_SHORTS
            while (wcount--)
                csum = (csum * 3) + *csptr++;
#else
            WavpackNativeToLittleEndian ((WavpackHeader *) buffer, WavpackHeaderFormat);

            while (wcount--) {
                csum = (csum * 3) + csptr [0] + (csptr [1] << 8);
                csptr += 2;
            }

            WavpackLittleEndianToNative ((WavpackHeader *) buffer, WavpackHeaderFormat);
 #endif
 
             if (meta_bc == 4) {
                if (*dp != (csum & 0xff) || dp[1] != ((csum >> 8) & 0xff) || dp[2] != ((csum >> 16) & 0xff) || dp[3] != ((csum >> 24) & 0xff))
                     return FALSE;
             }
             else {
                 csum ^= csum >> 16;
 
                if (*dp != (csum & 0xff) || dp[1] != ((csum >> 8) & 0xff))
                     return FALSE;
             }
 
            checksum_passed++;
        }

        bcount -= meta_bc;
        dp += meta_bc;
    }

    return (bcount == 0) && (!verify_checksum || !(wphdr->flags & HAS_CHECKSUM) || checksum_passed);
}
