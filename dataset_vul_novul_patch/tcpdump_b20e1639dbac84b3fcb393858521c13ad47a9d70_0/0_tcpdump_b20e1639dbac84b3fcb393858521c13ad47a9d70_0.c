isis_print_mt_port_cap_subtlv(netdissect_options *ndo,
                              const uint8_t *tptr, int len)
{
  int stlv_type, stlv_len;
  const struct isis_subtlv_spb_mcid *subtlv_spb_mcid;
  int i;
 
   while (len > 2)
   {
    ND_TCHECK2(*tptr, 2);
     stlv_type = *(tptr++);
     stlv_len  = *(tptr++);
 
    /* first lets see if we know the subTLVs name*/
    ND_PRINT((ndo, "\n\t       %s subTLV #%u, length: %u",
               tok2str(isis_mt_port_cap_subtlv_values, "unknown", stlv_type),
               stlv_type,
               stlv_len));

     /*len -= TLV_TYPE_LEN_OFFSET;*/
     len = len -2;
 
    /* Make sure the subTLV fits within the space left */
    if (len < stlv_len)
      goto trunc;
    /* Make sure the entire subTLV is in the captured data */
    ND_TCHECK2(*(tptr), stlv_len);

     switch (stlv_type)
     {
       case ISIS_SUBTLV_SPB_MCID:
       {
      	if (stlv_len < ISIS_SUBTLV_SPB_MCID_MIN_LEN)
      	  goto trunc;
 
         subtlv_spb_mcid = (const struct isis_subtlv_spb_mcid *)tptr;
 
        ND_PRINT((ndo,  "\n\t         MCID: "));
        isis_print_mcid(ndo, &(subtlv_spb_mcid->mcid));

          /*tptr += SPB_MCID_MIN_LEN;
            len -= SPB_MCID_MIN_LEN; */

        ND_PRINT((ndo,  "\n\t         AUX-MCID: "));
        isis_print_mcid(ndo, &(subtlv_spb_mcid->aux_mcid));
 
           /*tptr += SPB_MCID_MIN_LEN;
             len -= SPB_MCID_MIN_LEN; */
        tptr = tptr + ISIS_SUBTLV_SPB_MCID_MIN_LEN;
        len = len - ISIS_SUBTLV_SPB_MCID_MIN_LEN;
        stlv_len = stlv_len - ISIS_SUBTLV_SPB_MCID_MIN_LEN;
 
         break;
       }
 
       case ISIS_SUBTLV_SPB_DIGEST:
       {
        if (stlv_len < ISIS_SUBTLV_SPB_DIGEST_MIN_LEN)
          goto trunc;
 
         ND_PRINT((ndo, "\n\t        RES: %d V: %d A: %d D: %d",
                         (*(tptr) >> 5), (((*tptr)>> 4) & 0x01),
                        ((*(tptr) >> 2) & 0x03), ((*tptr) & 0x03)));

        tptr++;

        ND_PRINT((ndo,  "\n\t         Digest: "));

        for(i=1;i<=8; i++)
        {
            ND_PRINT((ndo, "%08x ", EXTRACT_32BITS(tptr)));
            if (i%4 == 0 && i != 8)
              ND_PRINT((ndo, "\n\t                 "));
            tptr = tptr + 4;
         }
 
         len = len - ISIS_SUBTLV_SPB_DIGEST_MIN_LEN;
        stlv_len = stlv_len - ISIS_SUBTLV_SPB_DIGEST_MIN_LEN;
 
         break;
       }
 
       case ISIS_SUBTLV_SPB_BVID:
       {
        while (stlv_len >= ISIS_SUBTLV_SPB_BVID_MIN_LEN)
         {
           ND_PRINT((ndo, "\n\t           ECT: %08x",
                       EXTRACT_32BITS(tptr)));
 
          tptr = tptr+4;

          ND_PRINT((ndo, " BVID: %d, U:%01x M:%01x ",
                     (EXTRACT_16BITS (tptr) >> 4) ,
                     (EXTRACT_16BITS (tptr) >> 3) & 0x01,
                     (EXTRACT_16BITS (tptr) >> 2) & 0x01));
 
           tptr = tptr + 2;
           len = len - ISIS_SUBTLV_SPB_BVID_MIN_LEN;
          stlv_len = stlv_len - ISIS_SUBTLV_SPB_BVID_MIN_LEN;
         }
 
         break;
       }
 
       default:
        break;
     }
    tptr += stlv_len;
    len -= stlv_len;
   }
 
   return 0;

  trunc:
    ND_PRINT((ndo, "\n\t\t"));
    ND_PRINT((ndo, "%s", tstr));
    return(1);
}
