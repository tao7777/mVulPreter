hash_foreach_prepend_string (gpointer key, gpointer val, gpointer user_data)
{
  HashAndString *data = (HashAndString*) user_data;
  gchar *in = (gchar*) val;
  g_hash_table_insert (data->hash, g_strdup ((gchar*) key),
                       g_strjoin (" ", data->string, in, NULL));
}
