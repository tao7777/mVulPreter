mp_capable_print(netdissect_options *ndo,
                 const u_char *opt, u_int opt_len, u_char flags)
 {
         const struct mp_capable *mpc = (const struct mp_capable *) opt;
 
        if (!(opt_len == 12 && (flags & TH_SYN)) &&
             !(opt_len == 20 && (flags & (TH_SYN | TH_ACK)) == TH_ACK))
                 return 0;
 
        if (MP_CAPABLE_OPT_VERSION(mpc->sub_ver) != 0) {
                ND_PRINT((ndo, " Unknown Version (%d)", MP_CAPABLE_OPT_VERSION(mpc->sub_ver)));
                return 1;
        }

        if (mpc->flags & MP_CAPABLE_C)
                ND_PRINT((ndo, " csum"));
        ND_PRINT((ndo, " {0x%" PRIx64, EXTRACT_64BITS(mpc->sender_key)));
        if (opt_len == 20) /* ACK */
                ND_PRINT((ndo, ",0x%" PRIx64, EXTRACT_64BITS(mpc->receiver_key)));
        ND_PRINT((ndo, "}"));
        return 1;
}
