 static void build_l4proto_tcp(const struct nf_conntrack *ct, struct nethdr *n)
 {
 	if (!nfct_attr_is_set(ct, ATTR_TCP_STATE))
 		return;
 
	ct_build_group(ct, ATTR_GRP_ORIG_PORT, n, NTA_PORT,
		      sizeof(struct nfct_attr_grp_port));
 	ct_build_u8(ct, ATTR_TCP_STATE, n, NTA_TCP_STATE);
 	if (CONFIG(sync).tcp_window_tracking) {
 		ct_build_u8(ct, ATTR_TCP_WSCALE_ORIG, n, NTA_TCP_WSCALE_ORIG);
		ct_build_u8(ct, ATTR_TCP_WSCALE_REPL, n, NTA_TCP_WSCALE_REPL);
	}
}
