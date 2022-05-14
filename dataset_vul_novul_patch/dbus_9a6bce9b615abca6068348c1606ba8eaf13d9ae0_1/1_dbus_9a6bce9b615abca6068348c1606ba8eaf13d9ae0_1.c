hash_foreach (gpointer key, gpointer val, gpointer user_data)
{
  const char *keystr = key;
  const char *valstr = val;
  guint *count = user_data;
  *count += (strlen (keystr) + strlen (valstr));
  g_print ("%s -> %s\n", keystr, valstr);
}
