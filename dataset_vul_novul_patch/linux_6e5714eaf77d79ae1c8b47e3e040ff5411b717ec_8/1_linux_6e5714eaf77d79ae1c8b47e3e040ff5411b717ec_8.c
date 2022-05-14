__u32 secure_tcpv6_sequence_number(__be32 *saddr, __be32 *daddr,
				   __be16 sport, __be16 dport)
{
	__u32 seq;
	__u32 hash[12];
	struct keydata *keyptr = get_keyptr();
	/* The procedure is the same as for IPv4, but addresses are longer.
	 * Thus we must use twothirdsMD4Transform.
	 */
	memcpy(hash, saddr, 16);
	hash[4] = ((__force u16)sport << 16) + (__force u16)dport;
	memcpy(&hash[5], keyptr->secret, sizeof(__u32) * 7);
	seq = twothirdsMD4Transform((const __u32 *)daddr, hash) & HASH_MASK;
	seq += keyptr->count;
	seq += ktime_to_ns(ktime_get_real());
	return seq;
}
