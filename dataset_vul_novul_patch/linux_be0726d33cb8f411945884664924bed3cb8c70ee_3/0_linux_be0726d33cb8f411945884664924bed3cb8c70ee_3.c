exit_ext2_xattr(void)
void ext2_xattr_destroy_cache(struct mb2_cache *cache)
 {
	if (cache)
		mb2_cache_destroy(cache);
 }
