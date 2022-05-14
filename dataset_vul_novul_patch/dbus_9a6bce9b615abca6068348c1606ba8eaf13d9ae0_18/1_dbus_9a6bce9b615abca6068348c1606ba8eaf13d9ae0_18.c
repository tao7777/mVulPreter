my_object_get_objs (MyObject *obj, GPtrArray **objs, GError **error)
{
  *objs = g_ptr_array_new ();
  g_ptr_array_add (*objs, g_strdup ("/org/freedesktop/DBus/GLib/Tests/MyTestObject"));
  g_ptr_array_add (*objs, g_strdup ("/org/freedesktop/DBus/GLib/Tests/MyTestObject2"));
  return TRUE;
}
