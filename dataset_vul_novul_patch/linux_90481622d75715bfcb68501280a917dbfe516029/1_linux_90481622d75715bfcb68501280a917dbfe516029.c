int hugetlb_get_quota(struct address_space *mapping, long delta)
{
	int ret = 0;
	struct hugetlbfs_sb_info *sbinfo = HUGETLBFS_SB(mapping->host->i_sb);
	if (sbinfo->free_blocks > -1) {
		spin_lock(&sbinfo->stat_lock);
		if (sbinfo->free_blocks - delta >= 0)
			sbinfo->free_blocks -= delta;
		else
			ret = -ENOMEM;
		spin_unlock(&sbinfo->stat_lock);
	}
	return ret;
}
