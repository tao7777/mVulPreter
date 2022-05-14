my_object_terminate (MyObject *obj, GError **error)
{
  g_main_loop_quit (loop);
  return TRUE;
}
