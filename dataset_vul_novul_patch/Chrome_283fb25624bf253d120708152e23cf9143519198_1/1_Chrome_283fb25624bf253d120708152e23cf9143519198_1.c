 static void CreateGroupColumnList(const TaskManagerModel* tm,
                                  const std::string column_name,
                                   const int index,
                                   const int length,
                                   DictionaryValue* val) {
  ListValue *list = new ListValue();
  for (int i = index; i < (index + length); i++) {
    list->Append(CreateColumnValue(tm, column_name, i));
  }
  val->Set(column_name, list);
}
