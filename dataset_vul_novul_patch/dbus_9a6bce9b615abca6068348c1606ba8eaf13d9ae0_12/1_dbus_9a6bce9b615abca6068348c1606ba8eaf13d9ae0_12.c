my_object_emit_signal2 (MyObject *obj, GError **error)
{
  GHashTable *table;
  table = g_hash_table_new (g_str_hash, g_str_equal);
  g_hash_table_insert (table, "baz", "cow");
  g_hash_table_insert (table, "bar", "foo");
  g_signal_emit (obj, signals[SIG2], 0, table);
  g_hash_table_destroy (table);
  return TRUE;
}
