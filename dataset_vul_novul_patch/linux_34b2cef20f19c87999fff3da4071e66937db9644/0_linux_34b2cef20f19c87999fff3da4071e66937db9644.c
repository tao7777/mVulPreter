void ipv4_pktinfo_prepare(const struct sock *sk, struct sk_buff *skb)
{
	struct in_pktinfo *pktinfo = PKTINFO_SKB_CB(skb);
	bool prepare = (inet_sk(sk)->cmsg_flags & IP_CMSG_PKTINFO) ||
		       ipv6_sk_rxinfo(sk);

	if (prepare && skb_rtable(skb)) {
		/* skb->cb is overloaded: prior to this point it is IP{6}CB
		 * which has interface index (iif) as the first member of the
		 * underlying inet{6}_skb_parm struct. This code then overlays
		 * PKTINFO_SKB_CB and in_pktinfo also has iif as the first
		 * element so the iif is picked up from the prior IPCB. If iif
		 * is the loopback interface, then return the sending interface
		 * (e.g., process binds socket to eth0 for Tx which is
		 * redirected to loopback in the rtable/dst).
		 */
		if (pktinfo->ipi_ifindex == LOOPBACK_IFINDEX)
			pktinfo->ipi_ifindex = inet_iif(skb);

		pktinfo->ipi_spec_dst.s_addr = fib_compute_spec_dst(skb);
	} else {
 		pktinfo->ipi_ifindex = 0;
 		pktinfo->ipi_spec_dst.s_addr = 0;
 	}
	/* We need to keep the dst for __ip_options_echo()
	 * We could restrict the test to opt.ts_needtime || opt.srr,
	 * but the following is good enough as IP options are not often used.
	 */
	if (unlikely(IPCB(skb)->opt.optlen))
		skb_dst_force(skb);
	else
		skb_dst_drop(skb);
 }
