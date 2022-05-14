grub_ext2_read_block (grub_fshelp_node_t node, grub_disk_addr_t fileblock)
{
  struct grub_ext2_data *data = node->data;
  struct grub_ext2_inode *inode = &node->inode;
  int blknr = -1;
  unsigned int blksz = EXT2_BLOCK_SIZE (data);
  int log2_blksz = LOG2_EXT2_BLOCK_SIZE (data);
 
   if (grub_le_to_cpu32(inode->flags) & EXT4_EXTENTS_FLAG)
     {
	  char * buf = grub_malloc (EXT2_BLOCK_SIZE (data));
          if (!buf) {
              return -1;
          }
       struct grub_ext4_extent_header *leaf;
       struct grub_ext4_extent *ext;
       int i;

      leaf = grub_ext4_find_leaf (data, buf,
		  (struct grub_ext4_extent_header *) inode->blocks.dir_blocks,
		  fileblock);
       if (! leaf)
         {
           grub_error (GRUB_ERR_BAD_FS, "invalid extent");
	  free (buf);
           return -1;
         }
 
      ext = (struct grub_ext4_extent *) (leaf + 1);
      for (i = 0; i < grub_le_to_cpu16 (leaf->entries); i++)
        {
          if (fileblock < grub_le_to_cpu32 (ext[i].block))
            break;
        }

       if (--i >= 0)
         {
           fileblock -= grub_le_to_cpu32 (ext[i].block);
          if (fileblock >= grub_le_to_cpu16 (ext[i].len)) {
  	    free (buf);
             return 0;
          } else
             {
               grub_disk_addr_t start;
 
               start = grub_le_to_cpu16 (ext[i].start_hi);
               start = (start << 32) + grub_le_to_cpu32 (ext[i].start);
  	    free (buf);
 
               return fileblock + start;
             }
         }
       else
         {
           grub_error (GRUB_ERR_BAD_FS, "something wrong with extent");
  	    free (buf);
           return -1;
         }
free (buf);
     }
   /* Direct blocks.  */
   if (fileblock < INDIRECT_BLOCKS) {
    blknr = grub_le_to_cpu32 (inode->blocks.dir_blocks[fileblock]);
  /* Indirect.  */
  } else if (fileblock < INDIRECT_BLOCKS + blksz / 4)
    {
       grub_uint32_t *indir;
 
       indir = grub_malloc (blksz);
      if (! indir) {
 	return grub_errno;
}
 
       if (grub_disk_read (data->disk,
 			  ((grub_disk_addr_t)
 			   grub_le_to_cpu32 (inode->blocks.indir_block))
 			  << log2_blksz,
			  0, blksz, indir)) {
 	return grub_errno;
}
 
       blknr = grub_le_to_cpu32 (indir[fileblock - INDIRECT_BLOCKS]);
       grub_free (indir);
    }
  /* Double indirect.  */
  else if (fileblock < (grub_disk_addr_t)(INDIRECT_BLOCKS + blksz / 4) \
		  * (grub_disk_addr_t)(blksz / 4 + 1))
    {
      unsigned int perblock = blksz / 4;
      unsigned int rblock = fileblock - (INDIRECT_BLOCKS
					 + blksz / 4);
       grub_uint32_t *indir;
 
       indir = grub_malloc (blksz);
      if (! indir) {
 	return grub_errno;
}
 
       if (grub_disk_read (data->disk,
 			  ((grub_disk_addr_t)
 			   grub_le_to_cpu32 (inode->blocks.double_indir_block))
 			  << log2_blksz,
			  0, blksz, indir)) {
 	return grub_errno;
}
 
       if (grub_disk_read (data->disk,
 			  ((grub_disk_addr_t)
 			   grub_le_to_cpu32 (indir[rblock / perblock]))
 			  << log2_blksz,
			  0, blksz, indir)) {
 	return grub_errno;
}
 
       blknr = grub_le_to_cpu32 (indir[rblock % perblock]);
             grub_free (indir);
    }
  /* triple indirect.  */
  else
    {
      grub_error (GRUB_ERR_NOT_IMPLEMENTED_YET,
		  "ext2fs doesn't support triple indirect blocks");
    }

  return blknr;
}
