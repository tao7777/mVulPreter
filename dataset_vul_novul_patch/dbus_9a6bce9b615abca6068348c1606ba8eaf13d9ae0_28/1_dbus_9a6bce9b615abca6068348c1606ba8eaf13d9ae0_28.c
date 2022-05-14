my_object_many_return (MyObject *obj, guint32 *arg0, char **arg1, gint32 *arg2, guint32 *arg3, guint32 *arg4, const char **arg5, GError **error)
{
  *arg0 = 42;
  *arg1 = g_strdup ("42");
  *arg2 = -67;
  *arg3 = 2;
  *arg4 = 26;
  *arg5 = "hello world"; /* Annotation specifies as const */
  return TRUE;
}
