my_object_many_stringify (MyObject *obj, GHashTable /* char * -> GValue * */ *vals, GHashTable /* char * -> GValue * */ **ret, GError **error)
{
  *ret = g_hash_table_new_full (g_str_hash, g_str_equal,
				g_free, unset_and_free_gvalue);
  g_hash_table_foreach (vals, hash_foreach_stringify, *ret);
  return TRUE;
}
