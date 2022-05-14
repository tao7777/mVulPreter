std::map<int64, base::Time> GetExpectedMetaTimes() {
  std::map<int64, base::Time> expected_meta_times;
  const std::map<int64, int64>& expected_meta_proto_times =
      GetExpectedMetaProtoTimes();
  for (std::map<int64, int64>::const_iterator it =
           expected_meta_proto_times.begin();
       it != expected_meta_proto_times.end(); ++it) {
    expected_meta_times[it->first] =
        browser_sync::ProtoTimeToTime(it->second);
  }
  return expected_meta_times;
}
