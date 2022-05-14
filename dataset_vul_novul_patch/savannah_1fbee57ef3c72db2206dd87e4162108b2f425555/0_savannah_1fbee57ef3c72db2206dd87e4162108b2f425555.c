 stringprep_utf8_nfkc_normalize (const char *str, ssize_t len)
 {
  size_t n;

  if (len < 0)
    n = strlen (str);
  else
    n = len;

  if (u8_check ((const uint8_t *) str, n))
    return NULL;

   return g_utf8_normalize (str, len, G_NORMALIZE_NFKC);
 }
