plan_a (char const *filename)
{
  char const *s;
  char const *lim;
  char const **ptr;
  char *buffer;
  lin iline;
  size_t size = instat.st_size;

  /* Fail if the file size doesn't fit in a size_t,
     or if storage isn't available.  */
  if (! (size == instat.st_size
	 && (buffer = malloc (size ? size : (size_t) 1))))
    return false;

  /* Read the input file, but don't bother reading it if it's empty.
     When creating files, the files do not actually exist.  */
  if (size)
     {
       if (S_ISREG (instat.st_mode))
         {
	  int flags = O_RDONLY | binary_transput;
 	  size_t buffered = 0, n;
	  int ifd;

	  if (! follow_symlinks)
	    flags |= O_NOFOLLOW;
	  ifd = safe_open (filename, flags, 0);
 	  if (ifd < 0)
 	    pfatal ("can't open file %s", quotearg (filename));
 
		  /* Some non-POSIX hosts exaggerate st_size in text mode;
		     or the file may have shrunk!  */
		  size = buffered;
		  break;
		}
	      if (n == (size_t) -1)
		{
		  /* Perhaps size is too large for this host.  */
		  close (ifd);
		  free (buffer);
		  return false;
		}
	      buffered += n;
	    }

	  if (close (ifd) != 0)
	    read_fatal ();
	}
