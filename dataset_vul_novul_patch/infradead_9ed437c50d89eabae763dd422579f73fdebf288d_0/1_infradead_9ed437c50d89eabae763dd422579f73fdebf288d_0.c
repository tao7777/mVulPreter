static int jffs2_create(struct inode *dir_i, struct dentry *dentry, int mode,
			struct nameidata *nd)
{
	struct jffs2_raw_inode *ri;
        struct jffs2_inode_info *f, *dir_f;
        struct jffs2_sb_info *c;
        struct inode *inode;
        int ret;
 
        ri = jffs2_alloc_raw_inode();
		return -ENOMEM;

	c = JFFS2_SB_INFO(dir_i->i_sb);

 
        D1(printk(KERN_DEBUG "jffs2_create()\n"));
 
       inode = jffs2_new_inode(dir_i, mode, ri);
 
        if (IS_ERR(inode)) {
                D1(printk(KERN_DEBUG "jffs2_new_inode() failed\n"));
		return PTR_ERR(inode);
	}

	inode->i_op = &jffs2_file_inode_operations;
	inode->i_fop = &jffs2_file_operations;
	inode->i_mapping->a_ops = &jffs2_file_address_operations;
	inode->i_mapping->nrpages = 0;

	f = JFFS2_INODE_INFO(inode);
	dir_f = JFFS2_INODE_INFO(dir_i);

	ret = jffs2_do_create(c, dir_f, f, ri,
			      dentry->d_name.name, dentry->d_name.len);
                              dentry->d_name.name, dentry->d_name.len);
 
        if (ret)
               goto fail;
 
        ret = jffs2_init_security(inode, dir_i);
        if (ret)
               goto fail;
       ret = jffs2_init_acl(inode, dir_i);
        if (ret)
                goto fail;
 

	jffs2_free_raw_inode(ri);
	d_instantiate(dentry, inode);

	D1(printk(KERN_DEBUG "jffs2_create: Created ino #%lu with mode %o, nlink %d(%d). nrpages %ld\n",
		  inode->i_ino, inode->i_mode, inode->i_nlink, f->inocache->nlink, inode->i_mapping->nrpages));
                  inode->i_ino, inode->i_mode, inode->i_nlink, f->inocache->nlink, inode->i_mapping->nrpages));
        return 0;
 
  fail:
        make_bad_inode(inode);
        iput(inode);

/***********************************************************************/


static int jffs2_unlink(struct inode *dir_i, struct dentry *dentry)
{
	struct jffs2_sb_info *c = JFFS2_SB_INFO(dir_i->i_sb);
	struct jffs2_inode_info *dir_f = JFFS2_INODE_INFO(dir_i);
	struct jffs2_inode_info *dead_f = JFFS2_INODE_INFO(dentry->d_inode);
	int ret;
	uint32_t now = get_seconds();

	ret = jffs2_do_unlink(c, dir_f, dentry->d_name.name,
			      dentry->d_name.len, dead_f, now);
	if (dead_f->inocache)
		dentry->d_inode->i_nlink = dead_f->inocache->nlink;
	if (!ret)
		dir_i->i_mtime = dir_i->i_ctime = ITIME(now);
	return ret;
}
/***********************************************************************/


static int jffs2_link (struct dentry *old_dentry, struct inode *dir_i, struct dentry *dentry)
{
	struct jffs2_sb_info *c = JFFS2_SB_INFO(old_dentry->d_inode->i_sb);
	struct jffs2_inode_info *f = JFFS2_INODE_INFO(old_dentry->d_inode);
	struct jffs2_inode_info *dir_f = JFFS2_INODE_INFO(dir_i);
	int ret;
	uint8_t type;
	uint32_t now;

	/* Don't let people make hard links to bad inodes. */
	if (!f->inocache)
		return -EIO;

	if (S_ISDIR(old_dentry->d_inode->i_mode))
		return -EPERM;

	/* XXX: This is ugly */
	type = (old_dentry->d_inode->i_mode & S_IFMT) >> 12;
	if (!type) type = DT_REG;

	now = get_seconds();
	ret = jffs2_do_link(c, dir_f, f->inocache->ino, type, dentry->d_name.name, dentry->d_name.len, now);

	if (!ret) {
		down(&f->sem);
		old_dentry->d_inode->i_nlink = ++f->inocache->nlink;
		up(&f->sem);
		d_instantiate(dentry, old_dentry->d_inode);
		dir_i->i_mtime = dir_i->i_ctime = ITIME(now);
		atomic_inc(&old_dentry->d_inode->i_count);
	}
	return ret;
}

/***********************************************************************/

static int jffs2_symlink (struct inode *dir_i, struct dentry *dentry, const char *target)
{
	struct jffs2_inode_info *f, *dir_f;
	struct jffs2_sb_info *c;
	struct inode *inode;
	struct jffs2_raw_inode *ri;
	struct jffs2_raw_dirent *rd;
	struct jffs2_full_dnode *fn;
	struct jffs2_full_dirent *fd;
	int namelen;
	uint32_t alloclen;
        struct jffs2_full_dirent *fd;
        int namelen;
        uint32_t alloclen;
        int ret, targetlen = strlen(target);
 
        /* FIXME: If you care. We'd need to use frags for the target
	ri = jffs2_alloc_raw_inode();

	if (!ri)
		return -ENOMEM;

	c = JFFS2_SB_INFO(dir_i->i_sb);

	/* Try to reserve enough space for both node and dirent.
	 * Just the node will do for now, though
	 */
	namelen = dentry->d_name.len;
	ret = jffs2_reserve_space(c, sizeof(*ri) + targetlen, &alloclen,
				  ALLOC_NORMAL, JFFS2_SUMMARY_INODE_SIZE);

	if (ret) {
		jffs2_free_raw_inode(ri);
		return ret;
	}

	inode = jffs2_new_inode(dir_i, S_IFLNK | S_IRWXUGO, ri);
                return ret;
        }
 
       inode = jffs2_new_inode(dir_i, S_IFLNK | S_IRWXUGO, ri);
 
        if (IS_ERR(inode)) {
                jffs2_free_raw_inode(ri);
	inode->i_op = &jffs2_symlink_inode_operations;

	f = JFFS2_INODE_INFO(inode);

	inode->i_size = targetlen;
	ri->isize = ri->dsize = ri->csize = cpu_to_je32(inode->i_size);
	ri->totlen = cpu_to_je32(sizeof(*ri) + inode->i_size);
	ri->hdr_crc = cpu_to_je32(crc32(0, ri, sizeof(struct jffs2_unknown_node)-4));

	ri->compr = JFFS2_COMPR_NONE;
	ri->data_crc = cpu_to_je32(crc32(0, target, targetlen));
	ri->node_crc = cpu_to_je32(crc32(0, ri, sizeof(*ri)-8));

	fn = jffs2_write_dnode(c, f, ri, target, targetlen, ALLOC_NORMAL);

	jffs2_free_raw_inode(ri);

	if (IS_ERR(fn)) {
		/* Eeek. Wave bye bye */
		up(&f->sem);
		jffs2_complete_reservation(c);
		jffs2_clear_inode(inode);
		return PTR_ERR(fn);
                up(&f->sem);
                jffs2_complete_reservation(c);
                jffs2_clear_inode(inode);
                return PTR_ERR(fn);
        }
 
		jffs2_complete_reservation(c);
		jffs2_clear_inode(inode);
		return -ENOMEM;
	}
                up(&f->sem);
                jffs2_complete_reservation(c);
                jffs2_clear_inode(inode);
                return -ENOMEM;
        }
