my_object_unstringify (MyObject *obj, const char *str, GValue *value, GError **error)
{
  if (str[0] == '\0' || !g_ascii_isdigit (str[0])) {
    g_value_init (value, G_TYPE_STRING);
    g_value_set_string (value, str);
  } else {
    g_value_init (value, G_TYPE_INT);
    g_value_set_int (value, (int) g_ascii_strtoull (str, NULL, 10));
  } 
  return TRUE;
}
