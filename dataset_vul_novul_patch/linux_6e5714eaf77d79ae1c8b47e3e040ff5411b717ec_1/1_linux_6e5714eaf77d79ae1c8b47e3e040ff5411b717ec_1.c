static void rekey_seq_generator(struct work_struct *work)
 {
	struct keydata *keyptr = &ip_keydata[1 ^ (ip_cnt & 1)];
	get_random_bytes(keyptr->secret, sizeof(keyptr->secret));
	keyptr->count = (ip_cnt & COUNT_MASK) << HASH_BITS;
	smp_wmb();
	ip_cnt++;
	schedule_delayed_work(&rekey_work,
			      round_jiffies_relative(REKEY_INTERVAL));
}
