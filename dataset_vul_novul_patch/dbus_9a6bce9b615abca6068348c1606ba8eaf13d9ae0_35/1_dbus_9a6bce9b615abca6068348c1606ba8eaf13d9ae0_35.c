my_object_recursive2 (MyObject *obj, guint32 reqlen, GArray **ret, GError **error)
{
  guint32 val;
  GArray *array;
  array = g_array_new (FALSE, TRUE, sizeof (guint32));
  while (reqlen > 0) {
    val = 42;
    g_array_append_val (array, val);
    val = 26;
    g_array_append_val (array, val);
    reqlen--;
  }
  val = 2;
  g_array_append_val (array, val);
  *ret = array;
  return TRUE;
}
