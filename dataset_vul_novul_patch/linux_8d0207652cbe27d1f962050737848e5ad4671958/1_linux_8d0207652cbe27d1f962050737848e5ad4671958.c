xfs_file_splice_write(
	struct pipe_inode_info	*pipe,
	struct file		*outfilp,
	loff_t			*ppos,
	size_t			count,
	unsigned int		flags)
{
	struct inode		*inode = outfilp->f_mapping->host;
	struct xfs_inode	*ip = XFS_I(inode);
	int			ioflags = 0;
	ssize_t			ret;
	XFS_STATS_INC(xs_write_calls);
	if (outfilp->f_mode & FMODE_NOCMTIME)
		ioflags |= IO_INVIS;
	if (XFS_FORCED_SHUTDOWN(ip->i_mount))
		return -EIO;
	xfs_ilock(ip, XFS_IOLOCK_EXCL);
	trace_xfs_file_splice_write(ip, count, *ppos, ioflags);
	ret = generic_file_splice_write(pipe, outfilp, ppos, count, flags);
	if (ret > 0)
		XFS_STATS_ADD(xs_write_bytes, ret);
	xfs_iunlock(ip, XFS_IOLOCK_EXCL);
	return ret;
}
