void board_init_f_init_reserve(ulong base)
{
	struct global_data *gd_ptr;

	/*
	 * clear GD entirely and set it up.
	 * Use gd_ptr, as gd may not be properly set yet.
	 */

	gd_ptr = (struct global_data *)base;
	/* zero the area */
	memset(gd_ptr, '\0', sizeof(*gd));
	/* set GD unless architecture did it already */
#if !defined(CONFIG_ARM)
	arch_setup_gd(gd_ptr);
#endif

	if (CONFIG_IS_ENABLED(SYS_REPORT_STACK_F_USAGE))
		board_init_f_init_stack_protection_addr(base);

	/* next alloc will be higher by one GD plus 16-byte alignment */
	base += roundup(sizeof(struct global_data), 16);

	/*
	 * record early malloc arena start.
	 * Use gd as it is now properly set for all architectures.
	 */

 #if CONFIG_VAL(SYS_MALLOC_F_LEN)
 	/* go down one 'early malloc arena' */
 	gd->malloc_base = base;
	/* next alloc will be higher by one 'early malloc arena' size */
	base += CONFIG_VAL(SYS_MALLOC_F_LEN);
 #endif
 
 	if (CONFIG_IS_ENABLED(SYS_REPORT_STACK_F_USAGE))
		board_init_f_init_stack_protection();
}
