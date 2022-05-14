my_object_emit_frobnicate (MyObject *obj, GError **error)
{
  g_signal_emit (obj, signals[FROBNICATE], 0, 42);
  return TRUE;
}
