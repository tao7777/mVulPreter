find_file (const char *currpath, grub_fshelp_node_t currroot,
 	   grub_fshelp_node_t *currfound,
 	   struct grub_fshelp_find_file_closure *c)
 {
 	char *fpath = grub_malloc (grub_strlen (currpath) + 1);
   char *name = fpath;
   char *next;
   enum grub_fshelp_filetype type = GRUB_FSHELP_DIR;
  grub_fshelp_node_t currnode = currroot;
  grub_fshelp_node_t oldnode = currroot;

  c->currroot = currroot;

  grub_strncpy (fpath, currpath, grub_strlen (currpath) + 1);

  /* Remove all leading slashes.  */
  while (*name == '/')
    name++;

   if (! *name)
     {
       *currfound = currnode;
free (fpath);
       return 0;
     }
 
  for (;;)
    {
      int found;
      struct find_file_closure cc;

      /* Extract the actual part from the pathname.  */
      next = grub_strchr (name, '/');
      if (next)
	{
	  /* Remove all leading slashes.  */
	  while (*next == '/')
	    *(next++) = '\0';
	}

      /* At this point it is expected that the current node is a
	 directory, check if this is true.  */
       if (type != GRUB_FSHELP_DIR)
 	{
 	  free_node (currnode, c);
free (fpath);
 	  return grub_error (GRUB_ERR_BAD_FILE_TYPE, "not a directory");
 	}
 
      cc.name = name;
      cc.type = &type;
      cc.oldnode = &oldnode;
      cc.currnode = &currnode;
      /* Iterate over the directory.  */
       found = c->iterate_dir (currnode, iterate, &cc);
       if (! found)
 	{
	  if (grub_errno) {
free (fpath);
 	    return grub_errno;
}
 
 	  break;
 	}

      /* Read in the symlink and follow it.  */
      if (type == GRUB_FSHELP_SYMLINK)
	{
	  char *symlink;

	  /* Test if the symlink does not loop.  */
	  if (++(c->symlinknest) == 8)
 	    {
 	      free_node (currnode, c);
 	      free_node (oldnode, c);
free (fpath);
 	      return grub_error (GRUB_ERR_SYMLINK_LOOP,
 				 "too deep nesting of symlinks");
 	    }

	  symlink = c->read_symlink (currnode);
	  free_node (currnode, c);

 	  if (!symlink)
 	    {
 	      free_node (oldnode, c);
free (fpath);
 	      return grub_errno;
 	    }
 
	  /* The symlink is an absolute path, go back to the root inode.  */
	  if (symlink[0] == '/')
	    {
	      free_node (oldnode, c);
	      oldnode = c->rootnode;
	    }

	  /* Lookup the node the symlink points to.  */
	  find_file (symlink, oldnode, &currnode, c);
	  type = c->foundtype;
	  grub_free (symlink);

 	  if (grub_errno)
 	    {
 	      free_node (oldnode, c);
free (fpath);
 	      return grub_errno;
 	    }
 	}

      free_node (oldnode, c);

      /* Found the node!  */
      if (! next || *next == '\0')
 	{
 	  *currfound = currnode;
 	  c->foundtype = type;
free (fpath);
 	  return 0;
 	}
 
       name = next;
     }
 
free (fpath);
   return grub_error (GRUB_ERR_FILE_NOT_FOUND, "file not found");
 }
