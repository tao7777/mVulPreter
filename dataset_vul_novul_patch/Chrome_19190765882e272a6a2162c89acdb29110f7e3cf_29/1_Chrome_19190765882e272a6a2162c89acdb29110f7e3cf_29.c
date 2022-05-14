std::map<int64, int64> GetMetaProtoTimes(sqlite3* db_handle) {
  sqlite_utils::SQLStatement statement;
  statement.prepare(
      db_handle,
      "SELECT metahandle, mtime, server_mtime, ctime, server_ctime FROM metas");
  EXPECT_EQ(5, statement.column_count());
  std::map<int64, int64> meta_times;
  while (true) {
    int query_result = statement.step();
    if (query_result != SQLITE_ROW) {
      EXPECT_EQ(SQLITE_DONE, query_result);
      break;
    }
    int64 metahandle = statement.column_int64(0);
    int64 mtime = statement.column_int64(1);
    int64 server_mtime = statement.column_int64(2);
    int64 ctime = statement.column_int64(3);
    int64 server_ctime = statement.column_int64(4);
    EXPECT_EQ(mtime, server_mtime);
    EXPECT_EQ(mtime, ctime);
    EXPECT_EQ(mtime, server_ctime);
    meta_times[metahandle] = mtime;
  }
  return meta_times;
}
