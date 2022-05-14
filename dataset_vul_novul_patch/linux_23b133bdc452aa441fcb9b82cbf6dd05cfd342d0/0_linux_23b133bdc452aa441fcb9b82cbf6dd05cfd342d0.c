static int udf_read_inode(struct inode *inode, bool hidden_inode)
{
	struct buffer_head *bh = NULL;
	struct fileEntry *fe;
	struct extendedFileEntry *efe;
	uint16_t ident;
	struct udf_inode_info *iinfo = UDF_I(inode);
	struct udf_sb_info *sbi = UDF_SB(inode->i_sb);
	struct kernel_lb_addr *iloc = &iinfo->i_location;
	unsigned int link_count;
	unsigned int indirections = 0;
	int bs = inode->i_sb->s_blocksize;
	int ret = -EIO;

reread:
	if (iloc->logicalBlockNum >=
	    sbi->s_partmaps[iloc->partitionReferenceNum].s_partition_len) {
		udf_debug("block=%d, partition=%d out of range\n",
			  iloc->logicalBlockNum, iloc->partitionReferenceNum);
		return -EIO;
	}

	/*
	 * Set defaults, but the inode is still incomplete!
	 * Note: get_new_inode() sets the following on a new inode:
	 *      i_sb = sb
	 *      i_no = ino
	 *      i_flags = sb->s_flags
	 *      i_state = 0
	 * clean_inode(): zero fills and sets
	 *      i_count = 1
	 *      i_nlink = 1
	 *      i_op = NULL;
	 */
	bh = udf_read_ptagged(inode->i_sb, iloc, 0, &ident);
	if (!bh) {
		udf_err(inode->i_sb, "(ino %ld) failed !bh\n", inode->i_ino);
		return -EIO;
	}

	if (ident != TAG_IDENT_FE && ident != TAG_IDENT_EFE &&
	    ident != TAG_IDENT_USE) {
		udf_err(inode->i_sb, "(ino %ld) failed ident=%d\n",
			inode->i_ino, ident);
		goto out;
	}

	fe = (struct fileEntry *)bh->b_data;
	efe = (struct extendedFileEntry *)bh->b_data;

	if (fe->icbTag.strategyType == cpu_to_le16(4096)) {
		struct buffer_head *ibh;

		ibh = udf_read_ptagged(inode->i_sb, iloc, 1, &ident);
		if (ident == TAG_IDENT_IE && ibh) {
			struct kernel_lb_addr loc;
			struct indirectEntry *ie;

			ie = (struct indirectEntry *)ibh->b_data;
			loc = lelb_to_cpu(ie->indirectICB.extLocation);

			if (ie->indirectICB.extLength) {
				brelse(ibh);
				memcpy(&iinfo->i_location, &loc,
				       sizeof(struct kernel_lb_addr));
				if (++indirections > UDF_MAX_ICB_NESTING) {
					udf_err(inode->i_sb,
						"too many ICBs in ICB hierarchy"
						" (max %d supported)\n",
						UDF_MAX_ICB_NESTING);
					goto out;
				}
				brelse(bh);
				goto reread;
			}
		}
		brelse(ibh);
	} else if (fe->icbTag.strategyType != cpu_to_le16(4)) {
		udf_err(inode->i_sb, "unsupported strategy type: %d\n",
			le16_to_cpu(fe->icbTag.strategyType));
		goto out;
	}
	if (fe->icbTag.strategyType == cpu_to_le16(4))
		iinfo->i_strat4096 = 0;
	else /* if (fe->icbTag.strategyType == cpu_to_le16(4096)) */
		iinfo->i_strat4096 = 1;

	iinfo->i_alloc_type = le16_to_cpu(fe->icbTag.flags) &
							ICBTAG_FLAG_AD_MASK;
	iinfo->i_unique = 0;
	iinfo->i_lenEAttr = 0;
	iinfo->i_lenExtents = 0;
	iinfo->i_lenAlloc = 0;
	iinfo->i_next_alloc_block = 0;
	iinfo->i_next_alloc_goal = 0;
	if (fe->descTag.tagIdent == cpu_to_le16(TAG_IDENT_EFE)) {
		iinfo->i_efe = 1;
		iinfo->i_use = 0;
		ret = udf_alloc_i_data(inode, bs -
					sizeof(struct extendedFileEntry));
		if (ret)
			goto out;
		memcpy(iinfo->i_ext.i_data,
		       bh->b_data + sizeof(struct extendedFileEntry),
		       bs - sizeof(struct extendedFileEntry));
	} else if (fe->descTag.tagIdent == cpu_to_le16(TAG_IDENT_FE)) {
		iinfo->i_efe = 0;
		iinfo->i_use = 0;
		ret = udf_alloc_i_data(inode, bs - sizeof(struct fileEntry));
		if (ret)
			goto out;
		memcpy(iinfo->i_ext.i_data,
		       bh->b_data + sizeof(struct fileEntry),
		       bs - sizeof(struct fileEntry));
	} else if (fe->descTag.tagIdent == cpu_to_le16(TAG_IDENT_USE)) {
		iinfo->i_efe = 0;
		iinfo->i_use = 1;
		iinfo->i_lenAlloc = le32_to_cpu(
				((struct unallocSpaceEntry *)bh->b_data)->
				 lengthAllocDescs);
		ret = udf_alloc_i_data(inode, bs -
					sizeof(struct unallocSpaceEntry));
		if (ret)
			goto out;
		memcpy(iinfo->i_ext.i_data,
		       bh->b_data + sizeof(struct unallocSpaceEntry),
		       bs - sizeof(struct unallocSpaceEntry));
		return 0;
	}

	ret = -EIO;
	read_lock(&sbi->s_cred_lock);
	i_uid_write(inode, le32_to_cpu(fe->uid));
	if (!uid_valid(inode->i_uid) ||
	    UDF_QUERY_FLAG(inode->i_sb, UDF_FLAG_UID_IGNORE) ||
	    UDF_QUERY_FLAG(inode->i_sb, UDF_FLAG_UID_SET))
		inode->i_uid = UDF_SB(inode->i_sb)->s_uid;

	i_gid_write(inode, le32_to_cpu(fe->gid));
	if (!gid_valid(inode->i_gid) ||
	    UDF_QUERY_FLAG(inode->i_sb, UDF_FLAG_GID_IGNORE) ||
	    UDF_QUERY_FLAG(inode->i_sb, UDF_FLAG_GID_SET))
		inode->i_gid = UDF_SB(inode->i_sb)->s_gid;

	if (fe->icbTag.fileType != ICBTAG_FILE_TYPE_DIRECTORY &&
			sbi->s_fmode != UDF_INVALID_MODE)
		inode->i_mode = sbi->s_fmode;
	else if (fe->icbTag.fileType == ICBTAG_FILE_TYPE_DIRECTORY &&
			sbi->s_dmode != UDF_INVALID_MODE)
		inode->i_mode = sbi->s_dmode;
	else
		inode->i_mode = udf_convert_permissions(fe);
	inode->i_mode &= ~sbi->s_umask;
	read_unlock(&sbi->s_cred_lock);

	link_count = le16_to_cpu(fe->fileLinkCount);
	if (!link_count) {
		if (!hidden_inode) {
			ret = -ESTALE;
			goto out;
		}
		link_count = 1;
	}
	set_nlink(inode, link_count);

	inode->i_size = le64_to_cpu(fe->informationLength);
	iinfo->i_lenExtents = inode->i_size;

	if (iinfo->i_efe == 0) {
		inode->i_blocks = le64_to_cpu(fe->logicalBlocksRecorded) <<
			(inode->i_sb->s_blocksize_bits - 9);

		if (!udf_disk_stamp_to_time(&inode->i_atime, fe->accessTime))
			inode->i_atime = sbi->s_record_time;

		if (!udf_disk_stamp_to_time(&inode->i_mtime,
					    fe->modificationTime))
			inode->i_mtime = sbi->s_record_time;

		if (!udf_disk_stamp_to_time(&inode->i_ctime, fe->attrTime))
			inode->i_ctime = sbi->s_record_time;

		iinfo->i_unique = le64_to_cpu(fe->uniqueID);
		iinfo->i_lenEAttr = le32_to_cpu(fe->lengthExtendedAttr);
		iinfo->i_lenAlloc = le32_to_cpu(fe->lengthAllocDescs);
		iinfo->i_checkpoint = le32_to_cpu(fe->checkpoint);
	} else {
		inode->i_blocks = le64_to_cpu(efe->logicalBlocksRecorded) <<
		    (inode->i_sb->s_blocksize_bits - 9);

		if (!udf_disk_stamp_to_time(&inode->i_atime, efe->accessTime))
			inode->i_atime = sbi->s_record_time;

		if (!udf_disk_stamp_to_time(&inode->i_mtime,
					    efe->modificationTime))
			inode->i_mtime = sbi->s_record_time;

		if (!udf_disk_stamp_to_time(&iinfo->i_crtime, efe->createTime))
			iinfo->i_crtime = sbi->s_record_time;

		if (!udf_disk_stamp_to_time(&inode->i_ctime, efe->attrTime))
			inode->i_ctime = sbi->s_record_time;

		iinfo->i_unique = le64_to_cpu(efe->uniqueID);
		iinfo->i_lenEAttr = le32_to_cpu(efe->lengthExtendedAttr);
		iinfo->i_lenAlloc = le32_to_cpu(efe->lengthAllocDescs);
		iinfo->i_checkpoint = le32_to_cpu(efe->checkpoint);
 	}
 	inode->i_generation = iinfo->i_unique;
 
	/*
	 * Sanity check length of allocation descriptors and extended attrs to
	 * avoid integer overflows
	 */
	if (iinfo->i_lenEAttr > bs || iinfo->i_lenAlloc > bs)
		goto out;
	/* Now do exact checks */
	if (udf_file_entry_alloc_offset(inode) + iinfo->i_lenAlloc > bs)
		goto out;
 	/* Sanity checks for files in ICB so that we don't get confused later */
 	if (iinfo->i_alloc_type == ICBTAG_FLAG_AD_IN_ICB) {
 		/*
		 * For file in ICB data is stored in allocation descriptor
		 * so sizes should match
		 */
		if (iinfo->i_lenAlloc != inode->i_size)
			goto out;
		/* File in ICB has to fit in there... */
		if (inode->i_size > bs - udf_file_entry_alloc_offset(inode))
			goto out;
	}

	switch (fe->icbTag.fileType) {
	case ICBTAG_FILE_TYPE_DIRECTORY:
		inode->i_op = &udf_dir_inode_operations;
		inode->i_fop = &udf_dir_operations;
		inode->i_mode |= S_IFDIR;
		inc_nlink(inode);
		break;
	case ICBTAG_FILE_TYPE_REALTIME:
	case ICBTAG_FILE_TYPE_REGULAR:
	case ICBTAG_FILE_TYPE_UNDEF:
	case ICBTAG_FILE_TYPE_VAT20:
		if (iinfo->i_alloc_type == ICBTAG_FLAG_AD_IN_ICB)
			inode->i_data.a_ops = &udf_adinicb_aops;
		else
			inode->i_data.a_ops = &udf_aops;
		inode->i_op = &udf_file_inode_operations;
		inode->i_fop = &udf_file_operations;
		inode->i_mode |= S_IFREG;
		break;
	case ICBTAG_FILE_TYPE_BLOCK:
		inode->i_mode |= S_IFBLK;
		break;
	case ICBTAG_FILE_TYPE_CHAR:
		inode->i_mode |= S_IFCHR;
		break;
	case ICBTAG_FILE_TYPE_FIFO:
		init_special_inode(inode, inode->i_mode | S_IFIFO, 0);
		break;
	case ICBTAG_FILE_TYPE_SOCKET:
		init_special_inode(inode, inode->i_mode | S_IFSOCK, 0);
		break;
	case ICBTAG_FILE_TYPE_SYMLINK:
		inode->i_data.a_ops = &udf_symlink_aops;
		inode->i_op = &udf_symlink_inode_operations;
		inode->i_mode = S_IFLNK | S_IRWXUGO;
		break;
	case ICBTAG_FILE_TYPE_MAIN:
		udf_debug("METADATA FILE-----\n");
		break;
	case ICBTAG_FILE_TYPE_MIRROR:
		udf_debug("METADATA MIRROR FILE-----\n");
		break;
	case ICBTAG_FILE_TYPE_BITMAP:
		udf_debug("METADATA BITMAP FILE-----\n");
		break;
	default:
		udf_err(inode->i_sb, "(ino %ld) failed unknown file type=%d\n",
			inode->i_ino, fe->icbTag.fileType);
		goto out;
	}
	if (S_ISCHR(inode->i_mode) || S_ISBLK(inode->i_mode)) {
		struct deviceSpec *dsea =
			(struct deviceSpec *)udf_get_extendedattr(inode, 12, 1);
		if (dsea) {
			init_special_inode(inode, inode->i_mode,
				MKDEV(le32_to_cpu(dsea->majorDeviceIdent),
				      le32_to_cpu(dsea->minorDeviceIdent)));
			/* Developer ID ??? */
		} else
			goto out;
	}
	ret = 0;
out:
	brelse(bh);
	return ret;
}
