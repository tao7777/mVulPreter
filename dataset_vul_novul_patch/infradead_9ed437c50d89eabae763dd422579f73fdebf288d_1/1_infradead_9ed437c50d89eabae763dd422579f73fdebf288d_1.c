static int jffs2_do_setattr (struct inode *inode, struct iattr *iattr)
 {
        struct jffs2_full_dnode *old_metadata, *new_metadata;
        struct jffs2_inode_info *f = JFFS2_INODE_INFO(inode);
	struct jffs2_sb_info *c = JFFS2_SB_INFO(inode->i_sb);
	struct jffs2_raw_inode *ri;
	union jffs2_device_node dev;
	unsigned char *mdata = NULL;
	int mdatalen = 0;
        unsigned int ivalid;
        uint32_t alloclen;
        int ret;
        D1(printk(KERN_DEBUG "jffs2_setattr(): ino #%lu\n", inode->i_ino));
       ret = inode_change_ok(inode, iattr);
       if (ret)
               return ret;
 
        /* Special cases - we don't want more than one data node
           for these types on the medium at any time. So setattr
	/* Special cases - we don't want more than one data node
	   for these types on the medium at any time. So setattr
	   must read the original data associated with the node
	   (i.e. the device numbers or the target name) and write
	   it out again with the appropriate data attached */
	if (S_ISBLK(inode->i_mode) || S_ISCHR(inode->i_mode)) {
		/* For these, we don't actually need to read the old node */
		mdatalen = jffs2_encode_dev(&dev, inode->i_rdev);
		mdata = (char *)&dev;
		D1(printk(KERN_DEBUG "jffs2_setattr(): Writing %d bytes of kdev_t\n", mdatalen));
	} else if (S_ISLNK(inode->i_mode)) {
		down(&f->sem);
		mdatalen = f->metadata->size;
		mdata = kmalloc(f->metadata->size, GFP_USER);
		if (!mdata) {
			up(&f->sem);
			return -ENOMEM;
		}
		ret = jffs2_read_dnode(c, f, f->metadata, mdata, 0, mdatalen);
		if (ret) {
			up(&f->sem);
			kfree(mdata);
			return ret;
		}
		up(&f->sem);
		D1(printk(KERN_DEBUG "jffs2_setattr(): Writing %d bytes of symlink target\n", mdatalen));
	}

	ri = jffs2_alloc_raw_inode();
	if (!ri) {
		if (S_ISLNK(inode->i_mode))
			kfree(mdata);
		return -ENOMEM;
	}

	ret = jffs2_reserve_space(c, sizeof(*ri) + mdatalen, &alloclen,
				  ALLOC_NORMAL, JFFS2_SUMMARY_INODE_SIZE);
	if (ret) {
		jffs2_free_raw_inode(ri);
		if (S_ISLNK(inode->i_mode & S_IFMT))
			 kfree(mdata);
		return ret;
	}
	down(&f->sem);
	ivalid = iattr->ia_valid;

	ri->magic = cpu_to_je16(JFFS2_MAGIC_BITMASK);
	ri->nodetype = cpu_to_je16(JFFS2_NODETYPE_INODE);
	ri->totlen = cpu_to_je32(sizeof(*ri) + mdatalen);
	ri->hdr_crc = cpu_to_je32(crc32(0, ri, sizeof(struct jffs2_unknown_node)-4));

	ri->ino = cpu_to_je32(inode->i_ino);
	ri->version = cpu_to_je32(++f->highest_version);

	ri->uid = cpu_to_je16((ivalid & ATTR_UID)?iattr->ia_uid:inode->i_uid);
	ri->gid = cpu_to_je16((ivalid & ATTR_GID)?iattr->ia_gid:inode->i_gid);

	if (ivalid & ATTR_MODE)
		if (iattr->ia_mode & S_ISGID &&
		    !in_group_p(je16_to_cpu(ri->gid)) && !capable(CAP_FSETID))
			ri->mode = cpu_to_jemode(iattr->ia_mode & ~S_ISGID);
		else
			ri->mode = cpu_to_jemode(iattr->ia_mode);
	else
		ri->mode = cpu_to_jemode(inode->i_mode);


	ri->isize = cpu_to_je32((ivalid & ATTR_SIZE)?iattr->ia_size:inode->i_size);
	ri->atime = cpu_to_je32(I_SEC((ivalid & ATTR_ATIME)?iattr->ia_atime:inode->i_atime));
	ri->mtime = cpu_to_je32(I_SEC((ivalid & ATTR_MTIME)?iattr->ia_mtime:inode->i_mtime));
	ri->ctime = cpu_to_je32(I_SEC((ivalid & ATTR_CTIME)?iattr->ia_ctime:inode->i_ctime));

	ri->offset = cpu_to_je32(0);
	ri->csize = ri->dsize = cpu_to_je32(mdatalen);
	ri->compr = JFFS2_COMPR_NONE;
	if (ivalid & ATTR_SIZE && inode->i_size < iattr->ia_size) {
		/* It's an extension. Make it a hole node */
		ri->compr = JFFS2_COMPR_ZERO;
		ri->dsize = cpu_to_je32(iattr->ia_size - inode->i_size);
		ri->offset = cpu_to_je32(inode->i_size);
	}
	ri->node_crc = cpu_to_je32(crc32(0, ri, sizeof(*ri)-8));
	if (mdatalen)
		ri->data_crc = cpu_to_je32(crc32(0, mdata, mdatalen));
	else
		ri->data_crc = cpu_to_je32(0);

	new_metadata = jffs2_write_dnode(c, f, ri, mdata, mdatalen, ALLOC_NORMAL);
	if (S_ISLNK(inode->i_mode))
		kfree(mdata);

	if (IS_ERR(new_metadata)) {
		jffs2_complete_reservation(c);
		jffs2_free_raw_inode(ri);
		up(&f->sem);
		return PTR_ERR(new_metadata);
	}
	/* It worked. Update the inode */
	inode->i_atime = ITIME(je32_to_cpu(ri->atime));
	inode->i_ctime = ITIME(je32_to_cpu(ri->ctime));
	inode->i_mtime = ITIME(je32_to_cpu(ri->mtime));
	inode->i_mode = jemode_to_cpu(ri->mode);
	inode->i_uid = je16_to_cpu(ri->uid);
	inode->i_gid = je16_to_cpu(ri->gid);


	old_metadata = f->metadata;

	if (ivalid & ATTR_SIZE && inode->i_size > iattr->ia_size)
		jffs2_truncate_fragtree (c, &f->fragtree, iattr->ia_size);

	if (ivalid & ATTR_SIZE && inode->i_size < iattr->ia_size) {
		jffs2_add_full_dnode_to_inode(c, f, new_metadata);
		inode->i_size = iattr->ia_size;
		f->metadata = NULL;
	} else {
		f->metadata = new_metadata;
	}
	if (old_metadata) {
		jffs2_mark_node_obsolete(c, old_metadata->raw);
		jffs2_free_full_dnode(old_metadata);
	}
	jffs2_free_raw_inode(ri);

	up(&f->sem);
	jffs2_complete_reservation(c);

	/* We have to do the vmtruncate() without f->sem held, since
	   some pages may be locked and waiting for it in readpage().
	   We are protected from a simultaneous write() extending i_size
	   back past iattr->ia_size, because do_truncate() holds the
	   generic inode semaphore. */
	if (ivalid & ATTR_SIZE && inode->i_size > iattr->ia_size)
		vmtruncate(inode, iattr->ia_size);

	return 0;
}
