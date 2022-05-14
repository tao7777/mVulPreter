__u32 secure_ipv6_id(const __be32 daddr[4])
{
	const struct keydata *keyptr;
	__u32 hash[4];
	keyptr = get_keyptr();
	hash[0] = (__force __u32)daddr[0];
	hash[1] = (__force __u32)daddr[1];
	hash[2] = (__force __u32)daddr[2];
	hash[3] = (__force __u32)daddr[3];
	return half_md4_transform(hash, keyptr->secret);
}
