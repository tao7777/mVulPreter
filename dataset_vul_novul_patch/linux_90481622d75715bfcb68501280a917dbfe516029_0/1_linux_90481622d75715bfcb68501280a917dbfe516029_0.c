void hugetlb_put_quota(struct address_space *mapping, long delta)
{
	struct hugetlbfs_sb_info *sbinfo = HUGETLBFS_SB(mapping->host->i_sb);
	if (sbinfo->free_blocks > -1) {
		spin_lock(&sbinfo->stat_lock);
		sbinfo->free_blocks += delta;
		spin_unlock(&sbinfo->stat_lock);
	}
}
