  void AssertObserverCount(int added_count, int removed_count,
                            int changed_count) {
     ASSERT_EQ(added_count, added_count_);
     ASSERT_EQ(removed_count, removed_count_);
    ASSERT_EQ(changed_count, changed_count_);
  }
