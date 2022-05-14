void ip_options_build(struct sk_buff * skb, struct ip_options * opt,
 			    __be32 daddr, struct rtable *rt, int is_frag)
 {
 	unsigned char *iph = skb_network_header(skb);

	memcpy(&(IPCB(skb)->opt), opt, sizeof(struct ip_options));
	memcpy(iph+sizeof(struct iphdr), opt->__data, opt->optlen);
	opt = &(IPCB(skb)->opt);

	if (opt->srr)
		memcpy(iph+opt->srr+iph[opt->srr+1]-4, &daddr, 4);

	if (!is_frag) {
		if (opt->rr_needaddr)
			ip_rt_get_source(iph+opt->rr+iph[opt->rr+2]-5, rt);
		if (opt->ts_needaddr)
			ip_rt_get_source(iph+opt->ts+iph[opt->ts+2]-9, rt);
		if (opt->ts_needtime) {
			struct timespec tv;
			__be32 midtime;
			getnstimeofday(&tv);
			midtime = htonl((tv.tv_sec % 86400) * MSEC_PER_SEC + tv.tv_nsec / NSEC_PER_MSEC);
			memcpy(iph+opt->ts+iph[opt->ts+2]-5, &midtime, 4);
		}
		return;
	}
	if (opt->rr) {
		memset(iph+opt->rr, IPOPT_NOP, iph[opt->rr+1]);
		opt->rr = 0;
		opt->rr_needaddr = 0;
	}
	if (opt->ts) {
		memset(iph+opt->ts, IPOPT_NOP, iph[opt->ts+1]);
		opt->ts = 0;
		opt->ts_needaddr = opt->ts_needtime = 0;
	}
}
