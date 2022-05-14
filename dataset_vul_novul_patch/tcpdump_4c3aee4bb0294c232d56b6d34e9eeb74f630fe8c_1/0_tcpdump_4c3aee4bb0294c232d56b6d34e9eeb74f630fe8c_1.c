 mp_dss_print(netdissect_options *ndo,
              const u_char *opt, u_int opt_len, u_char flags)
 {
         const struct mp_dss *mdss = (const struct mp_dss *) opt;
 
        /* We need the flags, at a minimum. */
        if (opt_len < 4)
                return 0;

        if (flags & TH_SYN)
                 return 0;
 
         if (mdss->flags & MP_DSS_F)
                 ND_PRINT((ndo, " fin"));
 
         opt += 4;
        opt_len -= 4;
         if (mdss->flags & MP_DSS_A) {
                /* Ack present */
                 ND_PRINT((ndo, " ack "));
                /*
                 * If the a flag is set, we have an 8-byte ack; if it's
                 * clear, we have a 4-byte ack.
                 */
                 if (mdss->flags & MP_DSS_a) {
                        if (opt_len < 8)
                                return 0;
                         ND_PRINT((ndo, "%" PRIu64, EXTRACT_64BITS(opt)));
                         opt += 8;
                        opt_len -= 8;
                 } else {
                        if (opt_len < 4)
                                return 0;
                         ND_PRINT((ndo, "%u", EXTRACT_32BITS(opt)));
                         opt += 4;
                        opt_len -= 4;
                 }
         }
 
         if (mdss->flags & MP_DSS_M) {
                /*
                 * Data Sequence Number (DSN), Subflow Sequence Number (SSN),
                 * Data-Level Length present, and Checksum possibly present.
                 */
                 ND_PRINT((ndo, " seq "));
		/*
                 * If the m flag is set, we have an 8-byte NDS; if it's clear,
                 * we have a 4-byte DSN.
                 */
                 if (mdss->flags & MP_DSS_m) {
                        if (opt_len < 8)
                                return 0;
                         ND_PRINT((ndo, "%" PRIu64, EXTRACT_64BITS(opt)));
                         opt += 8;
                        opt_len -= 8;
                 } else {
                        if (opt_len < 4)
                                return 0;
                         ND_PRINT((ndo, "%u", EXTRACT_32BITS(opt)));
                         opt += 4;
                        opt_len -= 4;
                 }
                if (opt_len < 4)
                        return 0;
                 ND_PRINT((ndo, " subseq %u", EXTRACT_32BITS(opt)));
                 opt += 4;
                opt_len -= 4;
                if (opt_len < 2)
                        return 0;
                 ND_PRINT((ndo, " len %u", EXTRACT_16BITS(opt)));
                 opt += 2;
                opt_len -= 2;
 
                /*
                 * The Checksum is present only if negotiated.
                 * If there are at least 2 bytes left, process the next 2
                 * bytes as the Checksum.
                 */
                if (opt_len >= 2) {
                         ND_PRINT((ndo, " csum 0x%x", EXTRACT_16BITS(opt)));
                        opt_len -= 2;
                }
         }
        if (opt_len != 0)
                return 0;
         return 1;
 }
