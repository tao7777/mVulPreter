my_object_throw_error (MyObject *obj, GError **error)
{
  g_set_error (error,
	       MY_OBJECT_ERROR,
	       MY_OBJECT_ERROR_FOO,
	       "%s",
	       "this method always loses");    
  return FALSE;
}
