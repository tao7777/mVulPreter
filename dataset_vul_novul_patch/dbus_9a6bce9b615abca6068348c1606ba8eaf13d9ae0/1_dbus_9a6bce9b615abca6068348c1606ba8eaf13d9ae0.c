do_async_error (IncrementData *data)
{
  GError *error;
  error = g_error_new (MY_OBJECT_ERROR,
		       MY_OBJECT_ERROR_FOO,
		       "%s",
		       "this method always loses");
  dbus_g_method_return_error (data->context, error);
  g_free (data);
  return FALSE;
}
