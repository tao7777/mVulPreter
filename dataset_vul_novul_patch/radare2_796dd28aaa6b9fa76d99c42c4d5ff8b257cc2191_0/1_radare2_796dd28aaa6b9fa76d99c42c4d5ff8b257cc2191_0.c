grub_ext2_iterate_dir (grub_fshelp_node_t dir,
		       int (*hook) (const char *filename,
				    enum grub_fshelp_filetype filetype,
				    grub_fshelp_node_t node,
				    void *closure),
		       void *closure)
{
  unsigned int fpos = 0;
  struct grub_fshelp_node *diro = (struct grub_fshelp_node *) dir;

  if (! diro->inode_read)
    {
      grub_ext2_read_inode (diro->data, diro->ino, &diro->inode);
      if (grub_errno)
	return 0;
    }

  /* Search the file.  */
  if (hook)
  while (fpos < grub_le_to_cpu32 (diro->inode.size))
    {
      struct ext2_dirent dirent;

      grub_ext2_read_file (diro, NULL, NULL, 0, fpos, sizeof (dirent),
			   (char *) &dirent);
      if (grub_errno)
	return 0;

      if (dirent.direntlen == 0)
        return 0;
 
       if (dirent.namelen != 0)
 	{
#ifndef _MSC_VER
	  char filename[dirent.namelen + 1]; 
#else
 	  char * filename = grub_malloc (dirent.namelen + 1);
#endif
 	  struct grub_fshelp_node *fdiro;
 	  enum grub_fshelp_filetype type = GRUB_FSHELP_UNKNOWN;
 
 	  grub_ext2_read_file (diro, 0, 0, 0,
 			       fpos + sizeof (struct ext2_dirent),
 			       dirent.namelen, filename);
	  if (grub_errno)
 	    return 0;
 
 	  fdiro = grub_malloc (sizeof (struct grub_fshelp_node));
	  if (! fdiro)
 	    return 0;
 
 	  fdiro->data = diro->data;
 	  fdiro->ino = grub_le_to_cpu32 (dirent.inode);

	  filename[dirent.namelen] = '\0';

	  if (dirent.filetype != FILETYPE_UNKNOWN)
	    {
	      fdiro->inode_read = 0;

	      if (dirent.filetype == FILETYPE_DIRECTORY)
		type = GRUB_FSHELP_DIR;
	      else if (dirent.filetype == FILETYPE_SYMLINK)
		type = GRUB_FSHELP_SYMLINK;
	      else if (dirent.filetype == FILETYPE_REG)
		type = GRUB_FSHELP_REG;
	    }
	  else
	    {
	      /* The filetype can not be read from the dirent, read
		 the inode to get more information.  */
 	      grub_ext2_read_inode (diro->data,
                                     grub_le_to_cpu32 (dirent.inode),
 				    &fdiro->inode);
	      if (grub_errno)
		{
 		  grub_free (fdiro);
 		  return 0;
 		}

	      fdiro->inode_read = 1;

	      if ((grub_le_to_cpu16 (fdiro->inode.mode)
		   & FILETYPE_INO_MASK) == FILETYPE_INO_DIRECTORY)
		type = GRUB_FSHELP_DIR;
	      else if ((grub_le_to_cpu16 (fdiro->inode.mode)
			& FILETYPE_INO_MASK) == FILETYPE_INO_SYMLINK)
		type = GRUB_FSHELP_SYMLINK;
	      else if ((grub_le_to_cpu16 (fdiro->inode.mode)
			& FILETYPE_INO_MASK) == FILETYPE_INO_REG)
 		type = GRUB_FSHELP_REG;
 	    }
 
	  if (hook (filename, type, fdiro, closure))
 	    return 1;
 	}
 
       fpos += grub_le_to_cpu16 (dirent.direntlen);
    }

  return 0;
}
