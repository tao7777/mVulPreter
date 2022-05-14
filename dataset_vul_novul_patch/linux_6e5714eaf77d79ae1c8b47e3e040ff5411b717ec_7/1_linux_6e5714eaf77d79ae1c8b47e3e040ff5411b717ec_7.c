__u32 secure_tcp_sequence_number(__be32 saddr, __be32 daddr,
				 __be16 sport, __be16 dport)
{
	__u32 seq;
	__u32 hash[4];
	struct keydata *keyptr = get_keyptr();
	/*
	 *  Pick a unique starting offset for each TCP connection endpoints
	 *  (saddr, daddr, sport, dport).
	 *  Note that the words are placed into the starting vector, which is
	 *  then mixed with a partial MD4 over random data.
	 */
	hash[0] = (__force u32)saddr;
	hash[1] = (__force u32)daddr;
	hash[2] = ((__force u16)sport << 16) + (__force u16)dport;
	hash[3] = keyptr->secret[11];
	seq = half_md4_transform(hash, keyptr->secret) & HASH_MASK;
	seq += keyptr->count;
	/*
	 *	As close as possible to RFC 793, which
	 *	suggests using a 250 kHz clock.
	 *	Further reading shows this assumes 2 Mb/s networks.
	 *	For 10 Mb/s Ethernet, a 1 MHz clock is appropriate.
	 *	For 10 Gb/s Ethernet, a 1 GHz clock should be ok, but
	 *	we also need to limit the resolution so that the u32 seq
	 *	overlaps less than one time per MSL (2 minutes).
	 *	Choosing a clock of 64 ns period is OK. (period of 274 s)
	 */
	seq += ktime_to_ns(ktime_get_real()) >> 6;
	return seq;
}
