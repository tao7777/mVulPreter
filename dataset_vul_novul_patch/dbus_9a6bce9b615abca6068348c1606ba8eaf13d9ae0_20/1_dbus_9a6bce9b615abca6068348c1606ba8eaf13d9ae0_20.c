my_object_get_val (MyObject *obj, guint *ret, GError **error)
{
  *ret = obj->val;
  return TRUE;
}
