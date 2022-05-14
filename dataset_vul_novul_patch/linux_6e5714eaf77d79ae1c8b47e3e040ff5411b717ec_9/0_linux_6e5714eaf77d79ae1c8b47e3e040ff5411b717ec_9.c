static __init int seqgen_init(void)
	get_random_bytes(random_int_secret, sizeof(random_int_secret));
 	return 0;
 }
