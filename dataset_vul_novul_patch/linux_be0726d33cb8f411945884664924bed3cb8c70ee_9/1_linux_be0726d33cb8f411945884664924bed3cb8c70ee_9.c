ext2_xattr_put_super(struct super_block *sb)
{
	mb_cache_shrink(sb->s_bdev);
}
