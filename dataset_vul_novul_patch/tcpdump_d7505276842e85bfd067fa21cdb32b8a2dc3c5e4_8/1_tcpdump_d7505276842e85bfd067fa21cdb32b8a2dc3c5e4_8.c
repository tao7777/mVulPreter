rpl_dio_printopt(netdissect_options *ndo,
                 const struct rpl_dio_genoption *opt,
                 u_int length)
{
        if(length < RPL_DIO_GENOPTION_LEN) return;
        length -= RPL_DIO_GENOPTION_LEN;

        ND_TCHECK(opt->rpl_dio_len);

        while((opt->rpl_dio_type == RPL_OPT_PAD0 &&
               (const u_char *)opt < ndo->ndo_snapend) ||
              ND_TTEST2(*opt,(opt->rpl_dio_len+2))) {

                unsigned int optlen = opt->rpl_dio_len+2;
                if(opt->rpl_dio_type == RPL_OPT_PAD0) {
                        optlen = 1;
                        ND_PRINT((ndo, " opt:pad0"));
                } else {
                        ND_PRINT((ndo, " opt:%s len:%u ",
                                  tok2str(rpl_subopt_values, "subopt:%u", opt->rpl_dio_type),
                                  optlen));
                        if(ndo->ndo_vflag > 2) {
                                unsigned int paylen = opt->rpl_dio_len;
                                if(paylen > length) paylen = length;
                                hex_print(ndo,
                                          " ",
                                          ((const uint8_t *)opt) + RPL_DIO_GENOPTION_LEN,  /* content of DIO option */
                                          paylen);
                        }
                 }
                 opt = (const struct rpl_dio_genoption *)(((const char *)opt) + optlen);
                 length -= optlen;
         }
         return;
 trunc:
	ND_PRINT((ndo," [|truncated]"));
 	return;
 }
