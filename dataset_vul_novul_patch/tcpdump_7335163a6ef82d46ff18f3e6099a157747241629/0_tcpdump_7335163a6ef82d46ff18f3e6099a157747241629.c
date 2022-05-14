handle_mlppp(netdissect_options *ndo,
             const u_char *p, int length)
{
     if (!ndo->ndo_eflag)
         ND_PRINT((ndo, "MLPPP, "));
 
    if (length < 2) {
        ND_PRINT((ndo, "[|mlppp]"));
        return;
    }
    if (!ND_TTEST_16BITS(p)) {
        ND_PRINT((ndo, "[|mlppp]"));
        return;
    }

     ND_PRINT((ndo, "seq 0x%03x, Flags [%s], length %u",
            (EXTRACT_16BITS(p))&0x0fff, /* only support 12-Bit sequence space for now */
            bittok2str(ppp_ml_flag_values, "none", *p & 0xc0),
           length));
}
