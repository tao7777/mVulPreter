 unsigned long lh_char_hash(const void *k)
 {
	static volatile int random_seed = -1;

	if (random_seed == -1) {
		int seed;
		/* we can't use -1 as it is the unitialized sentinel */
		while ((seed = json_c_get_random_seed()) == -1);
#if defined __GNUC__
		__sync_val_compare_and_swap(&random_seed, -1, seed);
#elif defined _MSC_VER
		InterlockedCompareExchange(&random_seed, seed, -1);
#else
#warning "racy random seed initializtion if used by multiple threads"
		random_seed = seed; /* potentially racy */
#endif
	}
 
	return hashlittle((const char*)k, strlen((const char*)k), random_seed); 
 }
