__u32 secure_ip_id(__be32 daddr)
{
	struct keydata *keyptr;
	__u32 hash[4];
	keyptr = get_keyptr();
	/*
	 *  Pick a unique starting offset for each IP destination.
	 *  The dest ip address is placed in the starting vector,
	 *  which is then hashed with random data.
	 */
	hash[0] = (__force __u32)daddr;
	hash[1] = keyptr->secret[9];
	hash[2] = keyptr->secret[10];
	hash[3] = keyptr->secret[11];
	return half_md4_transform(hash, keyptr->secret);
}
