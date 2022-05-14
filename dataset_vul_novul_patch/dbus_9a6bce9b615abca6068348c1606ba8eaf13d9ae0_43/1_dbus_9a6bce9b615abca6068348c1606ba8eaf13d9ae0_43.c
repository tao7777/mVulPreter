my_object_uppercase (MyObject *obj, const char *str, char **ret, GError **error)
{
  *ret = g_ascii_strup (str, -1);
  return TRUE;
}
