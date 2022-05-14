static inline struct keydata *get_keyptr(void)
{
	struct keydata *keyptr = &ip_keydata[ip_cnt & 1];
	smp_rmb();
	return keyptr;
}
