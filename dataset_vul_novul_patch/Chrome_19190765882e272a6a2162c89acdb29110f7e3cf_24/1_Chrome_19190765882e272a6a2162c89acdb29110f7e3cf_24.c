void ExpectTime(const EntryKernel& entry_kernel,
                const base::Time& expected_time) {
  EXPECT_PRED_FORMAT2(AssertTimesMatch,
                      expected_time, entry_kernel.ref(CTIME));
  EXPECT_PRED_FORMAT2(AssertTimesMatch,
                      expected_time, entry_kernel.ref(SERVER_CTIME));
  EXPECT_PRED_FORMAT2(AssertTimesMatch,
                      expected_time, entry_kernel.ref(MTIME));
  EXPECT_PRED_FORMAT2(AssertTimesMatch,
                      expected_time, entry_kernel.ref(SERVER_MTIME));
}
