 static int nntp_hcache_namer(const char *path, char *dest, size_t destlen)
 {
  int count = snprintf(dest, destlen, "%s.hcache", path);

  /* Strip out any directories in the path */
  char *first = strchr(dest, '/');
  char *last = strrchr(dest, '/');
  if (first && last && (last > first))
  {
    memmove(first, last, strlen(last) + 1);
    count -= (last - first);
  }

  return count;
 }
