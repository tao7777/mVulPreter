my_object_get_hash (MyObject *obj, GHashTable **ret, GError **error)
{
  GHashTable *table;
  table = g_hash_table_new (g_str_hash, g_str_equal);
  g_hash_table_insert (table, "foo", "bar");
  g_hash_table_insert (table, "baz", "whee");
  g_hash_table_insert (table, "cow", "crack");
  *ret = table;
  return TRUE;
}
