my_object_get_value (MyObject *obj, guint *ret, GError **error)
{
  *ret = obj->val;
  return TRUE;
}
