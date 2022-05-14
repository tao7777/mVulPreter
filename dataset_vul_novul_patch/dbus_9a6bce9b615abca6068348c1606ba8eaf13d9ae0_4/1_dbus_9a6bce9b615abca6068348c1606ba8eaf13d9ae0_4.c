hash_foreach_stringify (gpointer key, gpointer val, gpointer user_data)
{
  const char *keystr = key;
  const GValue *value = val;
  GValue *sval;
  GHashTable *ret = user_data;
  sval = g_new0 (GValue, 1);
  g_value_init (sval, G_TYPE_STRING);
  if (!g_value_transform (value, sval))
    g_assert_not_reached ();
  g_hash_table_insert (ret, g_strdup (keystr), sval);
}
