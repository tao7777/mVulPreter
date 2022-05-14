my_object_stringify (MyObject *obj, GValue *value, char **ret, GError **error)
{
  GValue valstr = {0, };
  g_value_init (&valstr, G_TYPE_STRING);
  if (!g_value_transform (value, &valstr))
    {
      g_set_error (error,
		   MY_OBJECT_ERROR,
		   MY_OBJECT_ERROR_FOO,
		   "couldn't transform value");
      return FALSE;
    }
  *ret = g_value_dup_string (&valstr);
  g_value_unset (&valstr);
  return TRUE;
}
