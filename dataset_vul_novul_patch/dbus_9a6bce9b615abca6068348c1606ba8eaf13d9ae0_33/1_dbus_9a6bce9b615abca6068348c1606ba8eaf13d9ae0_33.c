my_object_rec_arrays (MyObject *obj, GPtrArray *in, GPtrArray **ret, GError **error)
{
  char **strs;
  GArray *ints;
  guint v_UINT;
  if (in->len != 2)
    {
      g_set_error (error,
		   MY_OBJECT_ERROR,
		   MY_OBJECT_ERROR_FOO,
		   "invalid array len");
      return FALSE;
    }
  strs = g_ptr_array_index (in, 0);
  if (!*strs || strcmp (*strs, "foo"))
    {
      g_set_error (error,
		   MY_OBJECT_ERROR,
		   MY_OBJECT_ERROR_FOO,
		   "invalid string 0");
      return FALSE;
    }
  strs++;
  if (!*strs || strcmp (*strs, "bar"))
    {
      g_set_error (error,
		   MY_OBJECT_ERROR,
		   MY_OBJECT_ERROR_FOO,
		   "invalid string 1");
      return FALSE;
    }
  strs++;
  if (*strs)
    {
      g_set_error (error,
		   MY_OBJECT_ERROR,
		   MY_OBJECT_ERROR_FOO,
		   "invalid string array len in pos 0");
      return FALSE;
    }
  strs = g_ptr_array_index (in, 1);
  if (!*strs || strcmp (*strs, "baz"))
    {
      g_set_error (error,
		   MY_OBJECT_ERROR,
		   MY_OBJECT_ERROR_FOO,
		   "invalid string 0");
      return FALSE;
    }
  strs++;
  if (!*strs || strcmp (*strs, "whee"))
    {
      g_set_error (error,
		   MY_OBJECT_ERROR,
		   MY_OBJECT_ERROR_FOO,
		   "invalid string 1");
      return FALSE;
    }
  strs++;
  if (!*strs || strcmp (*strs, "moo"))
    {
      g_set_error (error,
		   MY_OBJECT_ERROR,
		   MY_OBJECT_ERROR_FOO,
		   "invalid string 2");
      return FALSE;
    }
  strs++;
  if (*strs)
    {
      g_set_error (error,
		   MY_OBJECT_ERROR,
		   MY_OBJECT_ERROR_FOO,
		   "invalid string array len in pos 1");
      return FALSE;
    }
  *ret = g_ptr_array_new ();
  ints = g_array_new (TRUE, TRUE, sizeof (guint));
  v_UINT = 10;
  g_array_append_val (ints, v_UINT);
  v_UINT = 42;
  g_array_append_val (ints, v_UINT);
  v_UINT = 27;
  g_array_append_val (ints, v_UINT);
  g_ptr_array_add (*ret, ints);
  ints = g_array_new (TRUE, TRUE, sizeof (guint));
  v_UINT = 30;
  g_array_append_val (ints, v_UINT);
  g_ptr_array_add (*ret, ints);
  return TRUE;
}
