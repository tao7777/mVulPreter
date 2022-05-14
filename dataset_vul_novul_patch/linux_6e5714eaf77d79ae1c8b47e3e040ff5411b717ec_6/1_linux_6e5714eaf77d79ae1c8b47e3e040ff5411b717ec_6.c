u32 secure_ipv6_port_ephemeral(const __be32 *saddr, const __be32 *daddr,
			       __be16 dport)
{
	struct keydata *keyptr = get_keyptr();
	u32 hash[12];
	memcpy(hash, saddr, 16);
	hash[4] = (__force u32)dport;
	memcpy(&hash[5], keyptr->secret, sizeof(__u32) * 7);
	return twothirdsMD4Transform((const __u32 *)daddr, hash);
}
