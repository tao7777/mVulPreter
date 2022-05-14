my_object_emit_signals (MyObject *obj, GError **error)
{
  GValue val = {0, };
  g_signal_emit (obj, signals[SIG0], 0, "foo", 22, "moo");
  g_value_init (&val, G_TYPE_STRING);
  g_value_set_string (&val, "bar");
  g_signal_emit (obj, signals[SIG1], 0, "baz", &val);
  g_value_unset (&val);
  return TRUE;
}
