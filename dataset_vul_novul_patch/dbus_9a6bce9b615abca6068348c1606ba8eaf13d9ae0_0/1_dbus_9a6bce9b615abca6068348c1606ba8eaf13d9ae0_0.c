do_async_increment (IncrementData *data)
{
  gint32 newx = data->x + 1;
  dbus_g_method_return (data->context, newx);
  g_free (data);
  return FALSE;
}
