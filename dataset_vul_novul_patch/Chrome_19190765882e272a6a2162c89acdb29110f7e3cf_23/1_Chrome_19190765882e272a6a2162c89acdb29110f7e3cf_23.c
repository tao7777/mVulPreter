::testing::AssertionResult AssertTimesMatch(const char* t1_expr,
                                            const char* t2_expr,
                                            const base::Time& t1,
                                            const base::Time& t2) {
  if (t1 == t2)
    return ::testing::AssertionSuccess();
  return ::testing::AssertionFailure()
      << t1_expr << " and " << t2_expr
      << " (internal values: " << t1.ToInternalValue()
      << " and " << t2.ToInternalValue()
      << ") (proto time: " << browser_sync::TimeToProtoTime(t1)
      << " and " << browser_sync::TimeToProtoTime(t2)
      << ") do not match";
}
