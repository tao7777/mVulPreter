 COMPAT_SYSCALL_DEFINE3(set_mempolicy, int, mode, compat_ulong_t __user *, nmask,
 		       compat_ulong_t, maxnode)
 {
 	unsigned long __user *nm = NULL;
 	unsigned long nr_bits, alloc_size;
 	DECLARE_BITMAP(bm, MAX_NUMNODES);

	nr_bits = min_t(unsigned long, maxnode-1, MAX_NUMNODES);
 	alloc_size = ALIGN(nr_bits, BITS_PER_LONG) / 8;
 
 	if (nmask) {
		if (compat_get_bitmap(bm, nmask, nr_bits))
			return -EFAULT;
 		nm = compat_alloc_user_space(alloc_size);
		if (copy_to_user(nm, bm, alloc_size))
			return -EFAULT;
 	}
 
 	return sys_set_mempolicy(mode, nm, nr_bits+1);
 }
