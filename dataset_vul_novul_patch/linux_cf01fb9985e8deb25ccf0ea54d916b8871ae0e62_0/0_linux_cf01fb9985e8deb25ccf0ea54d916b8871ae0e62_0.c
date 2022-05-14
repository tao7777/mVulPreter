 COMPAT_SYSCALL_DEFINE6(mbind, compat_ulong_t, start, compat_ulong_t, len,
 		       compat_ulong_t, mode, compat_ulong_t __user *, nmask,
 		       compat_ulong_t, maxnode, compat_ulong_t, flags)
 {
 	unsigned long __user *nm = NULL;
 	unsigned long nr_bits, alloc_size;
 	nodemask_t bm;

	nr_bits = min_t(unsigned long, maxnode-1, MAX_NUMNODES);
 	alloc_size = ALIGN(nr_bits, BITS_PER_LONG) / 8;
 
 	if (nmask) {
		if (compat_get_bitmap(nodes_addr(bm), nmask, nr_bits))
			return -EFAULT;
 		nm = compat_alloc_user_space(alloc_size);
		if (copy_to_user(nm, nodes_addr(bm), alloc_size))
			return -EFAULT;
 	}
 
 	return sys_mbind(start, len, mode, nm, nr_bits+1, flags);
 }
