int yr_object_array_set_item(
    YR_OBJECT* object,
    YR_OBJECT* item,
    int index)
{
  YR_OBJECT_ARRAY* array;

  int i;
  int count;

  assert(index >= 0);
  assert(object->type == OBJECT_TYPE_ARRAY);

  array = object_as_array(object);
 
   if (array->items == NULL)
   {
    count = 64;

    while (count <= index)
      count *= 2;
 
     array->items = (YR_ARRAY_ITEMS*) yr_malloc(
         sizeof(YR_ARRAY_ITEMS) + count * sizeof(YR_OBJECT*));

    if (array->items == NULL)
      return ERROR_INSUFFICIENT_MEMORY;

    memset(array->items->objects, 0, count * sizeof(YR_OBJECT*));

    array->items->count = count;
  }
   else if (index >= array->items->count)
   {
     count = array->items->count * 2;

    while (count <= index)
      count *= 2;

     array->items = (YR_ARRAY_ITEMS*) yr_realloc(
         array->items,
         sizeof(YR_ARRAY_ITEMS) + count * sizeof(YR_OBJECT*));

    if (array->items == NULL)
      return ERROR_INSUFFICIENT_MEMORY;

    for (i = array->items->count; i < count; i++)
      array->items->objects[i] = NULL;

    array->items->count = count;
  }

  item->parent = object;
  array->items->objects[index] = item;

  return ERROR_SUCCESS;
}
