my_object_increment_retval_error (MyObject *obj, gint32 x, GError **error)
{
  if (x + 1 > 10)
    {
      g_set_error (error,
		   MY_OBJECT_ERROR,
		   MY_OBJECT_ERROR_FOO,
		   "%s",
		   "x is bigger than 9");    
      return FALSE;
    }
  return x + 1;
}
