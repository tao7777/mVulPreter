static inline __u32 dccp_v6_init_sequence(struct sk_buff *skb)
 {
 	return secure_dccpv6_sequence_number(ipv6_hdr(skb)->daddr.s6_addr32,
 					     ipv6_hdr(skb)->saddr.s6_addr32,
					     dccp_hdr(skb)->dccph_dport,
					     dccp_hdr(skb)->dccph_sport     );

}
