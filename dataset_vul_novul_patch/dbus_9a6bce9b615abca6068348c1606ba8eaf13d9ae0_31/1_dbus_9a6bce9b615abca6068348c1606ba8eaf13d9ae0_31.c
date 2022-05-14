my_object_objpath (MyObject *obj, const char *incoming, const char **outgoing, GError **error)
{
  if (strcmp (incoming, "/org/freedesktop/DBus/GLib/Tests/MyTestObject"))
    {
      g_set_error (error,
		   MY_OBJECT_ERROR,
		   MY_OBJECT_ERROR_FOO,
		   "invalid incoming object");
      return FALSE;
    }
  *outgoing = "/org/freedesktop/DBus/GLib/Tests/MyTestObject2";
  return TRUE;
}
