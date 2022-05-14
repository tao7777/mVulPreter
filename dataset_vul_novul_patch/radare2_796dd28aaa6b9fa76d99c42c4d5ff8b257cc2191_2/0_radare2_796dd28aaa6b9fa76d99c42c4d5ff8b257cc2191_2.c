grub_fshelp_read_file (grub_disk_t disk, grub_fshelp_node_t node,
		       void (*read_hook) (grub_disk_addr_t sector,
					  unsigned offset,
					  unsigned length,
					  void *closure),
		       void *closure, int flags,
		       grub_off_t pos, grub_size_t len, char *buf,
		       grub_disk_addr_t (*get_block) (grub_fshelp_node_t node,
						      grub_disk_addr_t block),
		       grub_off_t filesize, int log2blocksize)
{
  grub_disk_addr_t i, blockcnt;
  int blocksize = 1 << (log2blocksize + GRUB_DISK_SECTOR_BITS);

  /* Adjust LEN so it we can't read past the end of the file.  */
   if (pos + len > filesize)
     len = filesize - pos;
 
  if (len < 1 || len == 0xffffffff) {
    return -1;
  }

   blockcnt = ((len + pos) + blocksize - 1) >>
     (log2blocksize + GRUB_DISK_SECTOR_BITS);
 
  for (i = pos >> (log2blocksize + GRUB_DISK_SECTOR_BITS); i < blockcnt; i++)
    {
      grub_disk_addr_t blknr;
      int blockoff = pos & (blocksize - 1);
      int blockend = blocksize;

      int skipfirst = 0;

      blknr = get_block (node, i);
      if (grub_errno)
	return -1;

      blknr = blknr << log2blocksize;

      /* Last block.  */
      if (i == blockcnt - 1)
	{
	  blockend = (len + pos) & (blocksize - 1);

	  /* The last portion is exactly blocksize.  */
	  if (! blockend)
	    blockend = blocksize;
	}

      /* First block.  */
      if (i == (pos >> (log2blocksize + GRUB_DISK_SECTOR_BITS)))
	{
	  skipfirst = blockoff;
	  blockend -= skipfirst;
	}

      /* If the block number is 0 this block is not stored on disk but
	 is zero filled instead.  */
      if (blknr)
	{
	  disk->read_hook = read_hook;
	  disk->closure = closure;

grub_hack_lastoff = blknr * 512;
	  grub_disk_read_ex (disk, blknr, skipfirst, blockend, buf, flags);
	  disk->read_hook = 0;
	  if (grub_errno)
	    return -1;
	}
      else if (buf)
	grub_memset (buf, 0, blockend);

      if (buf)
	buf += blocksize - skipfirst;
    }

  return len;
}
