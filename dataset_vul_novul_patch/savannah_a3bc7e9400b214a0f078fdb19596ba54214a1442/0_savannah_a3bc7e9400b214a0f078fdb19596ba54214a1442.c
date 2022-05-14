bgp_nlri_parse_vpnv4 (struct peer *peer, struct attr *attr, 
		      struct bgp_nlri *packet)
{
  u_char *pnt;
  u_char *lim;
  struct prefix p;
  int psize;
  int prefixlen;
  u_int16_t type;
  struct rd_as rd_as;
  struct rd_ip rd_ip;
  struct prefix_rd prd;
  u_char *tagpnt;

  /* Check peer status. */
  if (peer->status != Established)
    return 0;
  
  /* Make prefix_rd */
  prd.family = AF_UNSPEC;
  prd.prefixlen = 64;

   pnt = packet->nlri;
   lim = pnt + packet->length;
 
#define VPN_PREFIXLEN_MIN_BYTES (3 + 8) /* label + RD */
   for (; pnt < lim; pnt += psize)
     {
       /* Clear prefix structure. */

 
       /* Fetch prefix length. */
       prefixlen = *pnt++;
      p.family = afi2family (packet->afi);
       psize = PSIZE (prefixlen);
      
      /* sanity check against packet data */
      if (prefixlen < VPN_PREFIXLEN_MIN_BYTES*8 || (pnt + psize) > lim)
        {
          zlog_err ("prefix length (%d) is less than 88"
                    " or larger than received (%u)",
                    prefixlen, (uint)(lim-pnt));
          return -1;
        }
      
      /* sanity check against storage for the IP address portion */
      if ((psize - VPN_PREFIXLEN_MIN_BYTES) > (ssize_t) sizeof(p.u))
        {
          zlog_err ("prefix length (%d) exceeds prefix storage (%zu)",
                    prefixlen - VPN_PREFIXLEN_MIN_BYTES*8, sizeof(p.u));
          return -1;
        }
      
      /* Sanity check against max bitlen of the address family */
      if ((psize - VPN_PREFIXLEN_MIN_BYTES) > prefix_blen (&p))
        {
          zlog_err ("prefix length (%d) exceeds family (%u) max byte length (%u)",
                    prefixlen - VPN_PREFIXLEN_MIN_BYTES*8, 
                    p.family, prefix_blen (&p));
          return -1;
                  
        }
      
       /* Copyr label to prefix. */
      tagpnt = pnt;
 
       /* Copy routing distinguisher to rd. */
       memcpy (&prd.val, pnt + 3, 8);
      else if (type == RD_TYPE_IP)
	zlog_info ("prefix %ld:%s:%ld:%s/%d", label, inet_ntoa (rd_ip.ip),
		   rd_ip.val, inet_ntoa (p.u.prefix4), p.prefixlen);
#endif /* 0 */

      if (pnt + psize > lim)
	return -1;

      if (attr)
	bgp_update (peer, &p, attr, AFI_IP, SAFI_MPLS_VPN,
		    ZEBRA_ROUTE_BGP, BGP_ROUTE_NORMAL, &prd, tagpnt, 0);
      else
 	  return -1;
 	}
 
      p.prefixlen = prefixlen - VPN_PREFIXLEN_MIN_BYTES*8;
      memcpy (&p.u.prefix, pnt + VPN_PREFIXLEN_MIN_BYTES, 
              psize - VPN_PREFIXLEN_MIN_BYTES);
 
 #if 0
       if (type == RD_TYPE_AS)
}
