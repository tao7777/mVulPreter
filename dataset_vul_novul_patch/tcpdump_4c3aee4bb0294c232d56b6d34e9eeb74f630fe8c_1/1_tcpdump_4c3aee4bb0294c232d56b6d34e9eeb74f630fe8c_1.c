 mp_dss_print(netdissect_options *ndo,
              const u_char *opt, u_int opt_len, u_char flags)
 {
         const struct mp_dss *mdss = (const struct mp_dss *) opt;
 
        if ((opt_len != mp_dss_len(mdss, 1) &&
             opt_len != mp_dss_len(mdss, 0)) || flags & TH_SYN)
                 return 0;
 
         if (mdss->flags & MP_DSS_F)
                 ND_PRINT((ndo, " fin"));
 
         opt += 4;
         if (mdss->flags & MP_DSS_A) {
                 ND_PRINT((ndo, " ack "));
                 if (mdss->flags & MP_DSS_a) {
                         ND_PRINT((ndo, "%" PRIu64, EXTRACT_64BITS(opt)));
                         opt += 8;
                 } else {
                         ND_PRINT((ndo, "%u", EXTRACT_32BITS(opt)));
                         opt += 4;
                 }
         }
 
         if (mdss->flags & MP_DSS_M) {
                 ND_PRINT((ndo, " seq "));
                 if (mdss->flags & MP_DSS_m) {
                         ND_PRINT((ndo, "%" PRIu64, EXTRACT_64BITS(opt)));
                         opt += 8;
                 } else {
                         ND_PRINT((ndo, "%u", EXTRACT_32BITS(opt)));
                         opt += 4;
                 }
                 ND_PRINT((ndo, " subseq %u", EXTRACT_32BITS(opt)));
                 opt += 4;
                 ND_PRINT((ndo, " len %u", EXTRACT_16BITS(opt)));
                 opt += 2;
 
                if (opt_len == mp_dss_len(mdss, 1))
                         ND_PRINT((ndo, " csum 0x%x", EXTRACT_16BITS(opt)));
         }
         return 1;
 }
