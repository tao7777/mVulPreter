static struct fileIdentDesc *udf_find_entry(struct inode *dir,
					    const struct qstr *child,
					    struct udf_fileident_bh *fibh,
					    struct fileIdentDesc *cfi)
{
	struct fileIdentDesc *fi = NULL;
	loff_t f_pos;
	int block, flen;
	unsigned char *fname = NULL;
	unsigned char *nameptr;
	uint8_t lfi;
	uint16_t liu;
	loff_t size;
	struct kernel_lb_addr eloc;
	uint32_t elen;
	sector_t offset;
	struct extent_position epos = {};
	struct udf_inode_info *dinfo = UDF_I(dir);
	int isdotdot = child->len == 2 &&
		child->name[0] == '.' && child->name[1] == '.';

	size = udf_ext0_offset(dir) + dir->i_size;
	f_pos = udf_ext0_offset(dir);

	fibh->sbh = fibh->ebh = NULL;
	fibh->soffset = fibh->eoffset = f_pos & (dir->i_sb->s_blocksize - 1);
	if (dinfo->i_alloc_type != ICBTAG_FLAG_AD_IN_ICB) {
		if (inode_bmap(dir, f_pos >> dir->i_sb->s_blocksize_bits, &epos,
		    &eloc, &elen, &offset) != (EXT_RECORDED_ALLOCATED >> 30))
			goto out_err;
		block = udf_get_lb_pblock(dir->i_sb, &eloc, offset);
		if ((++offset << dir->i_sb->s_blocksize_bits) < elen) {
			if (dinfo->i_alloc_type == ICBTAG_FLAG_AD_SHORT)
				epos.offset -= sizeof(struct short_ad);
			else if (dinfo->i_alloc_type == ICBTAG_FLAG_AD_LONG)
				epos.offset -= sizeof(struct long_ad);
		} else
			offset = 0;

		fibh->sbh = fibh->ebh = udf_tread(dir->i_sb, block);
		if (!fibh->sbh)
			goto out_err;
	}

	fname = kmalloc(UDF_NAME_LEN, GFP_NOFS);
	if (!fname)
		goto out_err;

	while (f_pos < size) {
		fi = udf_fileident_read(dir, &f_pos, fibh, cfi, &epos, &eloc,
					&elen, &offset);
		if (!fi)
			goto out_err;

		liu = le16_to_cpu(cfi->lengthOfImpUse);
		lfi = cfi->lengthFileIdent;

		if (fibh->sbh == fibh->ebh) {
			nameptr = fi->fileIdent + liu;
		} else {
			int poffset;	/* Unpaded ending offset */

			poffset = fibh->soffset + sizeof(struct fileIdentDesc) +
					liu + lfi;

			if (poffset >= lfi)
				nameptr = (uint8_t *)(fibh->ebh->b_data +
						      poffset - lfi);
			else {
				nameptr = fname;
				memcpy(nameptr, fi->fileIdent + liu,
					lfi - poffset);
				memcpy(nameptr + lfi - poffset,
					fibh->ebh->b_data, poffset);
			}
		}

		if ((cfi->fileCharacteristics & FID_FILE_CHAR_DELETED) != 0) {
			if (!UDF_QUERY_FLAG(dir->i_sb, UDF_FLAG_UNDELETE))
				continue;
		}

		if ((cfi->fileCharacteristics & FID_FILE_CHAR_HIDDEN) != 0) {
			if (!UDF_QUERY_FLAG(dir->i_sb, UDF_FLAG_UNHIDE))
				continue;
		}

		if ((cfi->fileCharacteristics & FID_FILE_CHAR_PARENT) &&
		    isdotdot)
			goto out_ok;

 		if (!lfi)
 			continue;
 
		flen = udf_get_filename(dir->i_sb, nameptr, fname, lfi);
 		if (flen && udf_match(flen, fname, child->len, child->name))
 			goto out_ok;
 	}

out_err:
	fi = NULL;
	if (fibh->sbh != fibh->ebh)
		brelse(fibh->ebh);
	brelse(fibh->sbh);
out_ok:
	brelse(epos.bh);
	kfree(fname);

	return fi;
}
