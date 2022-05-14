  void RunCommitOrderingTest(CommitOrderingTest* test) {
    ScopedDirLookup dir(syncdb_.manager(), syncdb_.name());
    ASSERT_TRUE(dir.good());

    map<int, syncable::Id> expected_positions;
    {  // Transaction scope.
      WriteTransaction trans(FROM_HERE, UNITTEST, dir);
      while (!test->id.IsRoot()) {
        if (test->commit_index >= 0) {
          map<int, syncable::Id>::value_type entry(test->commit_index,
                                                   test->id);
          bool double_position = !expected_positions.insert(entry).second;
          ASSERT_FALSE(double_position) << "Two id's expected at one position";
        }
        string utf8_name = test->id.GetServerId();
        string name(utf8_name.begin(), utf8_name.end());
        MutableEntry entry(&trans, CREATE, test->parent_id, name);

        entry.Put(syncable::ID, test->id);
        if (test->id.ServerKnows()) {
          entry.Put(BASE_VERSION, 5);
          entry.Put(SERVER_VERSION, 5);
          entry.Put(SERVER_PARENT_ID, test->parent_id);
        }
        entry.Put(syncable::IS_DIR, true);
        entry.Put(syncable::IS_UNSYNCED, true);
         entry.Put(syncable::SPECIFICS, DefaultBookmarkSpecifics());
        int64 now_server_time = ClientTimeToServerTime(syncable::Now());
        int64 now_plus_30s = ServerTimeToClientTime(now_server_time + 30000);
        int64 now_minus_2h = ServerTimeToClientTime(now_server_time - 7200000);
         entry.Put(syncable::MTIME, now_plus_30s);
         for (size_t i = 0 ; i < arraysize(test->features) ; ++i) {
           switch (test->features[i]) {
            case LIST_END:
              break;
            case SYNCED:
              entry.Put(syncable::IS_UNSYNCED, false);
              break;
            case DELETED:
              entry.Put(syncable::IS_DEL, true);
              break;
            case OLD_MTIME:
              entry.Put(MTIME, now_minus_2h);
              break;
            case MOVED_FROM_ROOT:
              entry.Put(SERVER_PARENT_ID, trans.root_id());
              break;
            default:
              FAIL() << "Bad value in CommitOrderingTest list";
          }
        }
        test++;
      }
    }
    LoopSyncShare();
    ASSERT_TRUE(expected_positions.size() ==
                mock_server_->committed_ids().size());
    for (size_t i = 0; i < expected_positions.size(); ++i) {
      EXPECT_TRUE(1 == expected_positions.count(i));
      EXPECT_TRUE(expected_positions[i] == mock_server_->committed_ids()[i]);
    }
  }
