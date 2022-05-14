 int UnpackEntry(sqlite_utils::SQLStatement* statement, EntryKernel** kernel) {
   *kernel = NULL;
   int query_result = statement->step();
  if (SQLITE_ROW == query_result) {
    *kernel = new EntryKernel;
    (*kernel)->clear_dirty(NULL);
    DCHECK(statement->column_count() == static_cast<int>(FIELD_COUNT));
     int i = 0;
     for (i = BEGIN_FIELDS; i < INT64_FIELDS_END; ++i) {
       (*kernel)->put(static_cast<Int64Field>(i), statement->column_int64(i));
     }
     for ( ; i < ID_FIELDS_END; ++i) {
       (*kernel)->mutable_ref(static_cast<IdField>(i)).s_ =
           statement->column_string(i);
    }
    for ( ; i < BIT_FIELDS_END; ++i) {
      (*kernel)->put(static_cast<BitField>(i), (0 != statement->column_int(i)));
    }
    for ( ; i < STRING_FIELDS_END; ++i) {
      (*kernel)->put(static_cast<StringField>(i),
          statement->column_string(i));
    }
    for ( ; i < PROTO_FIELDS_END; ++i) {
       (*kernel)->mutable_ref(static_cast<ProtoField>(i)).ParseFromArray(
           statement->column_blob(i), statement->column_bytes(i));
     }
    ZeroFields((*kernel), i);
   } else {
    DCHECK(SQLITE_DONE == query_result);
     (*kernel) = NULL;
   }
   return query_result;
}
