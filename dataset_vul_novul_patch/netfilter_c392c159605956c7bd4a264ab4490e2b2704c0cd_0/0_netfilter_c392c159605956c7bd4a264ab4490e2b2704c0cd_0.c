 static void build_l4proto_icmp(const struct nf_conntrack *ct, struct nethdr *n)
 {
	/* This is also used by ICMPv6 and nf_conntrack_ipv6 is optional */
	if (!nfct_attr_is_set(ct, ATTR_ICMP_TYPE))
		return;

 	ct_build_u8(ct, ATTR_ICMP_TYPE, n, NTA_ICMP_TYPE);
 	ct_build_u8(ct, ATTR_ICMP_CODE, n, NTA_ICMP_CODE);
 	ct_build_u16(ct, ATTR_ICMP_ID, n, NTA_ICMP_ID);
	ct_build_group(ct, ATTR_GRP_ORIG_PORT, n, NTA_PORT,
		      sizeof(struct nfct_attr_grp_port));
}
