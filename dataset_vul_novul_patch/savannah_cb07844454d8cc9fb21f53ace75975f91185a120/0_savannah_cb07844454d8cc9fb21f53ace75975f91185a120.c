pax_decode_header (struct tar_sparse_file *file)
{
  if (file->stat_info->sparse_major > 0)
    {
      uintmax_t u;
      char nbuf[UINTMAX_STRSIZE_BOUND];
      union block *blk;
      char *p;
      size_t i;
      off_t start;
      
#define COPY_BUF(b,buf,src) do                                     \
 {                                                                 \
   char *endp = b->buffer + BLOCKSIZE;                             \
   char *dst = buf;                                                \
   do                                                              \
     {                                                             \
       if (dst == buf + UINTMAX_STRSIZE_BOUND -1)                  \
         {                                                         \
           ERROR ((0, 0, _("%s: numeric overflow in sparse archive member"), \
	          file->stat_info->orig_file_name));               \
           return false;                                           \
         }                                                         \
       if (src == endp)                                            \
 	 {                                                         \
 	   set_next_block_after (b);                               \
            b = find_next_block ();                                 \
           if (!b)                                                 \
             FATAL_ERROR ((0, 0, _("Unexpected EOF in archive"))); \
            src = b->buffer;                                        \
 	   endp = b->buffer + BLOCKSIZE;                           \
 	 }                                                         \
   while (*dst++ != '\n');                                         \
   dst[-1] = 0;                                                    \
 } while (0)

      start = current_block_ordinal ();
      set_next_block_after (current_header);
       start = current_block_ordinal ();
       set_next_block_after (current_header);
       blk = find_next_block ();
      if (!blk)
        FATAL_ERROR ((0, 0, _("Unexpected EOF in archive")));
       p = blk->buffer;
       COPY_BUF (blk,nbuf,p);
       if (!decode_num (&u, nbuf, TYPE_MAXIMUM (size_t)))
	}
      file->stat_info->sparse_map_size = u;
      file->stat_info->sparse_map = xcalloc (file->stat_info->sparse_map_size,
					     sizeof (*file->stat_info->sparse_map));
      file->stat_info->sparse_map_avail = 0;
      for (i = 0; i < file->stat_info->sparse_map_size; i++)
	{
	  struct sp_array sp;

	  COPY_BUF (blk,nbuf,p);
	  if (!decode_num (&u, nbuf, TYPE_MAXIMUM (off_t)))
	    {
	      ERROR ((0, 0, _("%s: malformed sparse archive member"),
		      file->stat_info->orig_file_name));
	      return false;
	    }
	  sp.offset = u;
	  COPY_BUF (blk,nbuf,p);
	  if (!decode_num (&u, nbuf, TYPE_MAXIMUM (off_t)))
	    {
	      ERROR ((0, 0, _("%s: malformed sparse archive member"),
		      file->stat_info->orig_file_name));
	      return false;
	    }
	  sp.numbytes = u;
	  sparse_add_map (file->stat_info, &sp);
	}
      set_next_block_after (blk);

      file->dumped_size += BLOCKSIZE * (current_block_ordinal () - start);
    }

  return true;
}
