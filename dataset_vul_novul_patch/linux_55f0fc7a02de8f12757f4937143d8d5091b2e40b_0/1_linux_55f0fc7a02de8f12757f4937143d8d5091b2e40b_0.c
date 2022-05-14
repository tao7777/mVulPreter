 static void ipv6_select_ident(struct frag_hdr *fhdr, struct rt6_info *rt)
 {
 	static u32 ip6_idents_hashrnd __read_mostly;
 	u32 hash, id;
 
 	net_get_random_once(&ip6_idents_hashrnd, sizeof(ip6_idents_hashrnd));
 
 	hash = __ipv6_addr_jhash(&rt->rt6i_dst.addr, ip6_idents_hashrnd);
 	hash = __ipv6_addr_jhash(&rt->rt6i_src.addr, hash);
 
 	id = ip_idents_reserve(hash, 1);
 	fhdr->identification = htonl(id);
}
