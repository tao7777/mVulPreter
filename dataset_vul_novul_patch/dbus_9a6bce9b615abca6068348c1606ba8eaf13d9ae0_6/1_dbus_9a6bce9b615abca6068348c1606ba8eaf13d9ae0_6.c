my_object_async_throw_error (MyObject *obj, DBusGMethodInvocation *context)
{
  IncrementData *data = g_new0(IncrementData, 1);
  data->context = context;
  g_idle_add ((GSourceFunc)do_async_error,  data);
}
