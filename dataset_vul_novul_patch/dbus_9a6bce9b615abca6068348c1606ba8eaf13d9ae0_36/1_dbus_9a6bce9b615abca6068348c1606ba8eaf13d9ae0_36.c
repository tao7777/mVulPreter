my_object_send_car (MyObject *obj, GValueArray *invals, GValueArray **outvals, GError **error)
{
  if (invals->n_values != 3
      || G_VALUE_TYPE (g_value_array_get_nth (invals, 0)) != G_TYPE_STRING
      || G_VALUE_TYPE (g_value_array_get_nth (invals, 1)) != G_TYPE_UINT
      || G_VALUE_TYPE (g_value_array_get_nth (invals, 2)) != G_TYPE_VALUE)
    {
      g_set_error (error,
		   MY_OBJECT_ERROR,
		   MY_OBJECT_ERROR_FOO,
		   "invalid incoming values");
      return FALSE;
    }
  *outvals = g_value_array_new (2);
  g_value_array_append (*outvals, NULL);
  g_value_init (g_value_array_get_nth (*outvals, (*outvals)->n_values - 1), G_TYPE_UINT);
  g_value_set_uint (g_value_array_get_nth (*outvals, (*outvals)->n_values - 1),
		    g_value_get_uint (g_value_array_get_nth (invals, 1)) + 1);
  g_value_array_append (*outvals, NULL);
  g_value_init (g_value_array_get_nth (*outvals, (*outvals)->n_values - 1), DBUS_TYPE_G_OBJECT_PATH);
  g_value_set_boxed (g_value_array_get_nth (*outvals, (*outvals)->n_values - 1),
		     g_strdup ("/org/freedesktop/DBus/GLib/Tests/MyTestObject2"));
  return TRUE;
}
