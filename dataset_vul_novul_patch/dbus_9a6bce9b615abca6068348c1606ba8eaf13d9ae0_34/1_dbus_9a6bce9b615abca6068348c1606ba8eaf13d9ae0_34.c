my_object_recursive1 (MyObject *obj, GArray *array, guint32 *len_ret, GError **error)
{
  *len_ret = array->len;
  return TRUE;
}
