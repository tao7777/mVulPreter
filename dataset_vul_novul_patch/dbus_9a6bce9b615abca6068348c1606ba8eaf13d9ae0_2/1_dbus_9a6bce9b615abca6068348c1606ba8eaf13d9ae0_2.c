hash_foreach_mangle_dict_of_strings (gpointer key, gpointer val, gpointer user_data)
{
  GHashTable *out = (GHashTable*) user_data;
  GHashTable *in_dict = (GHashTable *) val;
  HashAndString *data = g_new0 (HashAndString, 1);
  data->string = (gchar*) key;
  data->hash = g_hash_table_new_full (g_str_hash, g_str_equal,
                                            g_free, g_free);
  g_hash_table_foreach (in_dict, hash_foreach_prepend_string, data);
  g_hash_table_insert(out, g_strdup ((gchar*) key), data->hash);
}
