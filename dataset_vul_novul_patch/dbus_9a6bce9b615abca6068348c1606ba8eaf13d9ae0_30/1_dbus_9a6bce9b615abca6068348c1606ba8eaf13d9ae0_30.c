my_object_many_uppercase (MyObject *obj, const char * const *in, char ***out, GError **error)
{
  int len;
  int i;
  len = g_strv_length ((char**) in);
  *out = g_new0 (char *, len + 1);
  for (i = 0; i < len; i++)
    {
      (*out)[i] = g_ascii_strup (in[i], -1);
    }
  (*out)[i] = NULL;
  return TRUE;
}
