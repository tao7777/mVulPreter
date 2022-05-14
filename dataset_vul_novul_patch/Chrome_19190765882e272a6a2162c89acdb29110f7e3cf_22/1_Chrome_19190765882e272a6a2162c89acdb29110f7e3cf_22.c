 int UnpackEntry(sqlite_utils::SQLStatement* statement, EntryKernel** kernel) {
   *kernel = NULL;
   int query_result = statement->step();
  if (query_result == SQLITE_ROW) {
    *kernel = new EntryKernel();
    DCHECK_EQ(statement->column_count(), static_cast<int>(FIELD_COUNT));
     int i = 0;
     for (i = BEGIN_FIELDS; i < INT64_FIELDS_END; ++i) {
       (*kernel)->put(static_cast<Int64Field>(i), statement->column_int64(i));
     }
    for ( ; i < TIME_FIELDS_END; ++i) {
      (*kernel)->put(static_cast<TimeField>(i),
                     browser_sync::ProtoTimeToTime(
                         statement->column_int64(i)));
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
   } else {
    DCHECK_EQ(query_result, SQLITE_DONE);
     (*kernel) = NULL;
   }
   return query_result;
}
