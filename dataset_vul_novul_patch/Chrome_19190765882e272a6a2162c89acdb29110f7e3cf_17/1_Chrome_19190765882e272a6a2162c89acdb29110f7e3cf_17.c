void ExpectTimeValue(const base::Time& expected_value,
                     const DictionaryValue& value, const std::string& key) {
  std::string time_str;
  EXPECT_TRUE(value.GetString(key, &time_str));
  EXPECT_EQ(browser_sync::GetTimeDebugString(expected_value), time_str);
}
