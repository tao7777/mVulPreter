my_object_increment (MyObject *obj, gint32 x, gint32 *ret, GError **error)
{
  *ret = x +1;
  return TRUE;
}
