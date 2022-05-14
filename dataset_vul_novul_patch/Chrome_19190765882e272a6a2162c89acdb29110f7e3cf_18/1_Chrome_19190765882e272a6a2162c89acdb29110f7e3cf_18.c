int BindFields(const EntryKernel& entry,
               sqlite_utils::SQLStatement* statement) {
  int index = 0;
  int i = 0;
   for (i = BEGIN_FIELDS; i < INT64_FIELDS_END; ++i) {
     statement->bind_int64(index++, entry.ref(static_cast<Int64Field>(i)));
   }
  for ( ; i < TIME_FIELDS_END; ++i) {
    statement->bind_int64(index++,
                          browser_sync::TimeToProtoTime(
                              entry.ref(static_cast<TimeField>(i))));
  }
   for ( ; i < ID_FIELDS_END; ++i) {
     statement->bind_string(index++, entry.ref(static_cast<IdField>(i)).s_);
   }
  for ( ; i < BIT_FIELDS_END; ++i) {
    statement->bind_int(index++, entry.ref(static_cast<BitField>(i)));
  }
  for ( ; i < STRING_FIELDS_END; ++i) {
    statement->bind_string(index++, entry.ref(static_cast<StringField>(i)));
  }
  std::string temp;
  for ( ; i < PROTO_FIELDS_END; ++i) {
    entry.ref(static_cast<ProtoField>(i)).SerializeToString(&temp);
    statement->bind_blob(index++, temp.data(), temp.length());
  }
  return index;
}
