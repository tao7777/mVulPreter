int module_load(
    YR_SCAN_CONTEXT* context,
    YR_OBJECT* module_object,
    void* module_data,
    size_t module_data_size)
{
  set_integer(1, module_object, "constants.one");
  set_integer(2, module_object, "constants.two");
  set_string("foo", module_object, "constants.foo");
  set_string("", module_object, "constants.empty");

  set_integer(1, module_object, "struct_array[1].i");

   set_integer(0, module_object, "integer_array[%i]", 0);
   set_integer(1, module_object, "integer_array[%i]", 1);
   set_integer(2, module_object, "integer_array[%i]", 2);
  set_integer(256, module_object, "integer_array[%i]", 256);
 
   set_string("foo", module_object, "string_array[%i]", 0);
   set_string("bar", module_object, "string_array[%i]", 1);
  set_string("baz", module_object, "string_array[%i]", 2);

  set_sized_string("foo\0bar", 7, module_object, "string_array[%i]", 3);

  set_string("foo", module_object, "string_dict[%s]", "foo");
  set_string("bar", module_object, "string_dict[\"bar\"]");

  set_string("foo", module_object, "struct_dict[%s].s", "foo");
  set_integer(1, module_object, "struct_dict[%s].i", "foo");

  return ERROR_SUCCESS;
}
