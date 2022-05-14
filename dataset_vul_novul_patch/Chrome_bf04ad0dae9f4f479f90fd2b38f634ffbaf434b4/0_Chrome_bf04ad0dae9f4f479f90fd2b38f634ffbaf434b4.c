bool PPVarToNPVariant(PP_Var var, NPVariant* result) {
  switch (var.type) {
    case PP_VARTYPE_UNDEFINED:
      VOID_TO_NPVARIANT(*result);
      break;
    case PP_VARTYPE_NULL:
      NULL_TO_NPVARIANT(*result);
      break;
    case PP_VARTYPE_BOOL:
      BOOLEAN_TO_NPVARIANT(var.value.as_bool, *result);
      break;
    case PP_VARTYPE_INT32:
      INT32_TO_NPVARIANT(var.value.as_int, *result);
      break;
    case PP_VARTYPE_DOUBLE:
      DOUBLE_TO_NPVARIANT(var.value.as_double, *result);
      break;
    case PP_VARTYPE_STRING: {
      scoped_refptr<StringVar> string(StringVar::FromPPVar(var));
      if (!string) {
        VOID_TO_NPVARIANT(*result);
         return false;
       }
       const std::string& value = string->value();
      char* c_string = static_cast<char*>(malloc(value.size()));
      memcpy(c_string, value.data(), value.size());
      STRINGN_TO_NPVARIANT(c_string, value.size(), *result);
       break;
     }
     case PP_VARTYPE_OBJECT: {
      scoped_refptr<ObjectVar> object(ObjectVar::FromPPVar(var));
      if (!object) {
        VOID_TO_NPVARIANT(*result);
        return false;
      }
      OBJECT_TO_NPVARIANT(WebBindings::retainObject(object->np_object()),
                          *result);
      break;
    }
    case PP_VARTYPE_ARRAY:
    case PP_VARTYPE_DICTIONARY:
      VOID_TO_NPVARIANT(*result);
      break;
  }
  return true;
}
