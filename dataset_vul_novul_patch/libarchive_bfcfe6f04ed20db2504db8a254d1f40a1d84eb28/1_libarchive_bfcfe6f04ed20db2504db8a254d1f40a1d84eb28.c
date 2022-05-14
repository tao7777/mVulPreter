rar_read_ahead(struct archive_read *a, size_t min, ssize_t *avail)
{
  struct rar *rar = (struct rar *)(a->format->data);
  const void *h = __archive_read_ahead(a, min, avail);
  int ret;
  if (avail)
  {
    if (a->archive.read_data_is_posix_read && *avail > (ssize_t)a->archive.read_data_requested)
      *avail = a->archive.read_data_requested;
    if (*avail > rar->bytes_remaining)
      *avail = (ssize_t)rar->bytes_remaining;
    if (*avail < 0)
      return NULL;
     else if (*avail == 0 && rar->main_flags & MHD_VOLUME &&
       rar->file_flags & FHD_SPLIT_AFTER)
     {
       ret = archive_read_format_rar_read_header(a, a->entry);
       if (ret == (ARCHIVE_EOF))
       {
         rar->has_endarc_header = 1;
         ret = archive_read_format_rar_read_header(a, a->entry);
       }
       if (ret != (ARCHIVE_OK))
         return NULL;
       return rar_read_ahead(a, min, avail);
    }
  }
  return h;
}
