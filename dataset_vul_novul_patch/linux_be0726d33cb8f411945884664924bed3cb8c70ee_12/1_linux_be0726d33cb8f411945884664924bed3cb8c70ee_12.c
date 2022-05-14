init_ext2_xattr(void)
 {
	ext2_xattr_cache = mb_cache_create("ext2_xattr", 6);
	if (!ext2_xattr_cache)
		return -ENOMEM;
	return 0;
 }
