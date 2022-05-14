std::string print_valuetype(Value::ValueType e) {
  switch (e) {
    case Value::TYPE_NULL:
      return "NULL ";
    case Value::TYPE_BOOLEAN:
      return "BOOL";
    case Value::TYPE_INTEGER:
      return "INT";
    case Value::TYPE_DOUBLE:
      return "DOUBLE";
    case Value::TYPE_STRING:
      return "STRING";
    case Value::TYPE_BINARY:
      return "BIN";
    case Value::TYPE_DICTIONARY:
      return "DICT";
    case Value::TYPE_LIST:
      return "LIST";
    default:
      return "ERROR";
  }
}
