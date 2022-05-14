my_object_process_variant_of_array_of_ints123 (MyObject *obj, GValue *variant, GError **error)
{
  GArray *array;
  int i;
  int j;
  j = 0;
  array = (GArray *)g_value_get_boxed (variant);
  for (i = 0; i <= 2; i++)
    {
      j = g_array_index (array, int, i);
      if (j != i + 1)
        goto error;
    }
  return TRUE;
error:
  *error = g_error_new (MY_OBJECT_ERROR,
		       MY_OBJECT_ERROR_FOO,
		       "Error decoding a variant of type ai (i + 1 = %i, j = %i)",
		       i, j + 1);
  return FALSE;
}
