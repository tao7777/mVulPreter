 stringprep_utf8_nfkc_normalize (const char *str, ssize_t len)
 {
   return g_utf8_normalize (str, len, G_NORMALIZE_NFKC);
 }
