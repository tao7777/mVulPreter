my_object_async_increment (MyObject *obj, gint32 x, DBusGMethodInvocation *context)
{
  IncrementData *data = g_new0 (IncrementData, 1);
  data->x = x;
  data->context = context;
  g_idle_add ((GSourceFunc)do_async_increment, data);
}
