static inline bool cfs_rq_is_decayed(struct cfs_rq *cfs_rq)
{
	if (cfs_rq->load.weight)
		return false;
	if (cfs_rq->avg.load_sum)
		return false;
	if (cfs_rq->avg.util_sum)
		return false;
	if (cfs_rq->avg.runnable_load_sum)
		return false;
	return true;
}
