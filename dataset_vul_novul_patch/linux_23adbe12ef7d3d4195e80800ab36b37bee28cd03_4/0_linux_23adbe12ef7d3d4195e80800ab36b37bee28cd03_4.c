xfs_ioctl_setattr(
	xfs_inode_t		*ip,
	struct fsxattr		*fa,
	int			mask)
{
	struct xfs_mount	*mp = ip->i_mount;
	struct xfs_trans	*tp;
	unsigned int		lock_flags = 0;
	struct xfs_dquot	*udqp = NULL;
	struct xfs_dquot	*pdqp = NULL;
	struct xfs_dquot	*olddquot = NULL;
	int			code;

	trace_xfs_ioctl_setattr(ip);

	if (mp->m_flags & XFS_MOUNT_RDONLY)
		return XFS_ERROR(EROFS);
	if (XFS_FORCED_SHUTDOWN(mp))
		return XFS_ERROR(EIO);

	/*
	 * Disallow 32bit project ids when projid32bit feature is not enabled.
	 */
	if ((mask & FSX_PROJID) && (fa->fsx_projid > (__uint16_t)-1) &&
			!xfs_sb_version_hasprojid32bit(&ip->i_mount->m_sb))
		return XFS_ERROR(EINVAL);

	/*
	 * If disk quotas is on, we make sure that the dquots do exist on disk,
	 * before we start any other transactions. Trying to do this later
	 * is messy. We don't care to take a readlock to look at the ids
	 * in inode here, because we can't hold it across the trans_reserve.
	 * If the IDs do change before we take the ilock, we're covered
	 * because the i_*dquot fields will get updated anyway.
	 */
	if (XFS_IS_QUOTA_ON(mp) && (mask & FSX_PROJID)) {
		code = xfs_qm_vop_dqalloc(ip, ip->i_d.di_uid,
					 ip->i_d.di_gid, fa->fsx_projid,
					 XFS_QMOPT_PQUOTA, &udqp, NULL, &pdqp);
		if (code)
			return code;
	}

	/*
	 * For the other attributes, we acquire the inode lock and
	 * first do an error checking pass.
	 */
	tp = xfs_trans_alloc(mp, XFS_TRANS_SETATTR_NOT_SIZE);
	code = xfs_trans_reserve(tp, &M_RES(mp)->tr_ichange, 0, 0);
	if (code)
		goto error_return;

	lock_flags = XFS_ILOCK_EXCL;
	xfs_ilock(ip, lock_flags);

	/*
	 * CAP_FOWNER overrides the following restrictions:
	 *
	 * The user ID of the calling process must be equal
	 * to the file owner ID, except in cases where the
	 * CAP_FSETID capability is applicable.
	 */
	if (!inode_owner_or_capable(VFS_I(ip))) {
		code = XFS_ERROR(EPERM);
		goto error_return;
	}

	/*
	 * Do a quota reservation only if projid is actually going to change.
	 * Only allow changing of projid from init_user_ns since it is a
	 * non user namespace aware identifier.
	 */
	if (mask & FSX_PROJID) {
		if (current_user_ns() != &init_user_ns) {
			code = XFS_ERROR(EINVAL);
			goto error_return;
		}

		if (XFS_IS_QUOTA_RUNNING(mp) &&
		    XFS_IS_PQUOTA_ON(mp) &&
		    xfs_get_projid(ip) != fa->fsx_projid) {
			ASSERT(tp);
			code = xfs_qm_vop_chown_reserve(tp, ip, udqp, NULL,
						pdqp, capable(CAP_FOWNER) ?
						XFS_QMOPT_FORCE_RES : 0);
			if (code)	/* out of quota */
				goto error_return;
		}
	}

	if (mask & FSX_EXTSIZE) {
		/*
		 * Can't change extent size if any extents are allocated.
		 */
		if (ip->i_d.di_nextents &&
		    ((ip->i_d.di_extsize << mp->m_sb.sb_blocklog) !=
		     fa->fsx_extsize)) {
			code = XFS_ERROR(EINVAL);	/* EFBIG? */
			goto error_return;
		}

		/*
		 * Extent size must be a multiple of the appropriate block
		 * size, if set at all. It must also be smaller than the
		 * maximum extent size supported by the filesystem.
		 *
		 * Also, for non-realtime files, limit the extent size hint to
		 * half the size of the AGs in the filesystem so alignment
		 * doesn't result in extents larger than an AG.
		 */
		if (fa->fsx_extsize != 0) {
			xfs_extlen_t    size;
			xfs_fsblock_t   extsize_fsb;

			extsize_fsb = XFS_B_TO_FSB(mp, fa->fsx_extsize);
			if (extsize_fsb > MAXEXTLEN) {
				code = XFS_ERROR(EINVAL);
				goto error_return;
			}

			if (XFS_IS_REALTIME_INODE(ip) ||
			    ((mask & FSX_XFLAGS) &&
			    (fa->fsx_xflags & XFS_XFLAG_REALTIME))) {
				size = mp->m_sb.sb_rextsize <<
				       mp->m_sb.sb_blocklog;
			} else {
				size = mp->m_sb.sb_blocksize;
				if (extsize_fsb > mp->m_sb.sb_agblocks / 2) {
					code = XFS_ERROR(EINVAL);
					goto error_return;
				}
			}

			if (fa->fsx_extsize % size) {
				code = XFS_ERROR(EINVAL);
				goto error_return;
			}
		}
	}


	if (mask & FSX_XFLAGS) {
		/*
		 * Can't change realtime flag if any extents are allocated.
		 */
		if ((ip->i_d.di_nextents || ip->i_delayed_blks) &&
		    (XFS_IS_REALTIME_INODE(ip)) !=
		    (fa->fsx_xflags & XFS_XFLAG_REALTIME)) {
			code = XFS_ERROR(EINVAL);	/* EFBIG? */
			goto error_return;
		}

		/*
		 * If realtime flag is set then must have realtime data.
		 */
		if ((fa->fsx_xflags & XFS_XFLAG_REALTIME)) {
			if ((mp->m_sb.sb_rblocks == 0) ||
			    (mp->m_sb.sb_rextsize == 0) ||
			    (ip->i_d.di_extsize % mp->m_sb.sb_rextsize)) {
				code = XFS_ERROR(EINVAL);
				goto error_return;
			}
		}

		/*
		 * Can't modify an immutable/append-only file unless
		 * we have appropriate permission.
		 */
		if ((ip->i_d.di_flags &
				(XFS_DIFLAG_IMMUTABLE|XFS_DIFLAG_APPEND) ||
		     (fa->fsx_xflags &
				(XFS_XFLAG_IMMUTABLE | XFS_XFLAG_APPEND))) &&
		    !capable(CAP_LINUX_IMMUTABLE)) {
			code = XFS_ERROR(EPERM);
			goto error_return;
		}
	}

	xfs_trans_ijoin(tp, ip, 0);

	/*
	 * Change file ownership.  Must be the owner or privileged.
	 */
	if (mask & FSX_PROJID) {
		/*
		 * CAP_FSETID overrides the following restrictions:
		 *
		 * The set-user-ID and set-group-ID bits of a file will be
 		 * cleared upon successful return from chown()
 		 */
 		if ((ip->i_d.di_mode & (S_ISUID|S_ISGID)) &&
		    !capable_wrt_inode_uidgid(VFS_I(ip), CAP_FSETID))
 			ip->i_d.di_mode &= ~(S_ISUID|S_ISGID);
 
 		/*
		 * Change the ownerships and register quota modifications
		 * in the transaction.
		 */
		if (xfs_get_projid(ip) != fa->fsx_projid) {
			if (XFS_IS_QUOTA_RUNNING(mp) && XFS_IS_PQUOTA_ON(mp)) {
				olddquot = xfs_qm_vop_chown(tp, ip,
							&ip->i_pdquot, pdqp);
			}
			xfs_set_projid(ip, fa->fsx_projid);

			/*
			 * We may have to rev the inode as well as
			 * the superblock version number since projids didn't
			 * exist before DINODE_VERSION_2 and SB_VERSION_NLINK.
			 */
			if (ip->i_d.di_version == 1)
				xfs_bump_ino_vers2(tp, ip);
		}

	}

	if (mask & FSX_EXTSIZE)
		ip->i_d.di_extsize = fa->fsx_extsize >> mp->m_sb.sb_blocklog;
	if (mask & FSX_XFLAGS) {
		xfs_set_diflags(ip, fa->fsx_xflags);
		xfs_diflags_to_linux(ip);
	}

	xfs_trans_ichgtime(tp, ip, XFS_ICHGTIME_CHG);
	xfs_trans_log_inode(tp, ip, XFS_ILOG_CORE);

	XFS_STATS_INC(xs_ig_attrchg);

	/*
	 * If this is a synchronous mount, make sure that the
	 * transaction goes to disk before returning to the user.
	 * This is slightly sub-optimal in that truncates require
	 * two sync transactions instead of one for wsync filesystems.
	 * One for the truncate and one for the timestamps since we
	 * don't want to change the timestamps unless we're sure the
	 * truncate worked.  Truncates are less than 1% of the laddis
	 * mix so this probably isn't worth the trouble to optimize.
	 */
	if (mp->m_flags & XFS_MOUNT_WSYNC)
		xfs_trans_set_sync(tp);
	code = xfs_trans_commit(tp, 0);
	xfs_iunlock(ip, lock_flags);

	/*
	 * Release any dquot(s) the inode had kept before chown.
	 */
	xfs_qm_dqrele(olddquot);
	xfs_qm_dqrele(udqp);
	xfs_qm_dqrele(pdqp);

	return code;

 error_return:
	xfs_qm_dqrele(udqp);
	xfs_qm_dqrele(pdqp);
	xfs_trans_cancel(tp, 0);
	if (lock_flags)
		xfs_iunlock(ip, lock_flags);
	return code;
}
