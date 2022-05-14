init_ext2_xattr(void)
#define HASH_BUCKET_BITS 10

struct mb2_cache *ext2_xattr_create_cache(void)
 {
	return mb2_cache_create(HASH_BUCKET_BITS);
 }
