int yr_object_copy(
    YR_OBJECT* object,
    YR_OBJECT** object_copy)
{
  YR_OBJECT* copy;
  YR_OBJECT* o;

  YR_STRUCTURE_MEMBER* structure_member;
  YR_OBJECT_FUNCTION* func;
  YR_OBJECT_FUNCTION* func_copy;

  int i;

  *object_copy = NULL;

  FAIL_ON_ERROR(yr_object_create(
      object->type,
      object->identifier,
      NULL,
      &copy));

   switch(object->type)
   {
     case OBJECT_TYPE_INTEGER:
      ((YR_OBJECT_INTEGER*) copy)->value = ((YR_OBJECT_INTEGER*) object)->value;
       break;
 
     case OBJECT_TYPE_STRING:
      if (((YR_OBJECT_STRING*) object)->value != NULL)
      {
        ((YR_OBJECT_STRING*) copy)->value = sized_string_dup(
            ((YR_OBJECT_STRING*) object)->value);
      }
      else
      {
        ((YR_OBJECT_STRING*) copy)->value = NULL;
      }
      break;

    case OBJECT_TYPE_FLOAT:
      ((YR_OBJECT_DOUBLE*) copy)->value = ((YR_OBJECT_DOUBLE*) object)->value;
       break;
 
     case OBJECT_TYPE_FUNCTION:

      func = (YR_OBJECT_FUNCTION*) object;
      func_copy = (YR_OBJECT_FUNCTION*) copy;

      FAIL_ON_ERROR_WITH_CLEANUP(
        yr_object_copy(func->return_obj, &func_copy->return_obj),
        yr_object_destroy(copy));

      for (i = 0; i < MAX_OVERLOADED_FUNCTIONS; i++)
        func_copy->prototypes[i] = func->prototypes[i];

      break;

    case OBJECT_TYPE_STRUCTURE:

      structure_member = ((YR_OBJECT_STRUCTURE*) object)->members;

      while (structure_member != NULL)
      {
        FAIL_ON_ERROR_WITH_CLEANUP(
            yr_object_copy(structure_member->object, &o),
            yr_object_destroy(copy));

        FAIL_ON_ERROR_WITH_CLEANUP(
            yr_object_structure_set_member(copy, o),
            yr_free(o);
            yr_object_destroy(copy));

        structure_member = structure_member->next;
      }

      break;

    case OBJECT_TYPE_ARRAY:

      yr_object_copy(
        ((YR_OBJECT_ARRAY *) object)->prototype_item,
        &o);

      ((YR_OBJECT_ARRAY *)copy)->prototype_item = o;

      break;

    case OBJECT_TYPE_DICTIONARY:

      yr_object_copy(
        ((YR_OBJECT_DICTIONARY *) object)->prototype_item,
        &o);

      ((YR_OBJECT_DICTIONARY *)copy)->prototype_item = o;

      break;

    default:
      assert(FALSE);

  }

  *object_copy = copy;

  return ERROR_SUCCESS;
}
