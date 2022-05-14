unset_and_free_gvalue (gpointer val)
{
  g_value_unset (val);
  g_free (val);
}
