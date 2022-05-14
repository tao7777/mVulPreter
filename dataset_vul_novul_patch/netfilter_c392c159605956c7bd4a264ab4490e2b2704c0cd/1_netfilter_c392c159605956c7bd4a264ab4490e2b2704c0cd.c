 static void build_l4proto_dccp(const struct nf_conntrack *ct, struct nethdr *n)
 {
	ct_build_group(ct, ATTR_GRP_ORIG_PORT, n, NTA_PORT,
		      sizeof(struct nfct_attr_grp_port));
 	if (!nfct_attr_is_set(ct, ATTR_DCCP_STATE))
 		return;
 
 	ct_build_u8(ct, ATTR_DCCP_STATE, n, NTA_DCCP_STATE);
 	ct_build_u8(ct, ATTR_DCCP_ROLE, n, NTA_DCCP_ROLE);
 }
