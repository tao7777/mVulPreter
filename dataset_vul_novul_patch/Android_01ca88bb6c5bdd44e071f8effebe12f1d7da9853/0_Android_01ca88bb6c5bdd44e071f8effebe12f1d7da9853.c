UWORD32 ihevcd_cabac_decode_bypass_bins_egk(cab_ctxt_t *ps_cabac,
 bitstrm_t *ps_bitstrm,
                                            WORD32 k)
{

    UWORD32 u4_sym;
    WORD32 numones;
    WORD32 bin;

 /* Sanity checks */
    ASSERT((k >= 0));


     numones = k;
     bin = 1;
     u4_sym = 0;
    while(bin && (numones <= 16))
     {
         IHEVCD_CABAC_DECODE_BYPASS_BIN(bin, ps_cabac, ps_bitstrm);
         u4_sym += bin << numones++;
     }
 
     numones -= 1;
 
     if(numones)
     {
        UWORD32 u4_suffix;

        IHEVCD_CABAC_DECODE_BYPASS_BINS(u4_suffix, ps_cabac, ps_bitstrm, numones);
        u4_sym += u4_suffix;
 }
 return (u4_sym);
}
