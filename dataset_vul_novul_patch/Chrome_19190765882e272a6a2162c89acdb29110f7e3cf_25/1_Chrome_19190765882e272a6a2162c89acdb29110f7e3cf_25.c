void ExpectTimes(const MetahandlesIndex& index,
                 const std::map<int64, base::Time>& expected_times) {
  for (MetahandlesIndex::const_iterator it = index.begin();
       it != index.end(); ++it) {
    int64 meta_handle = (*it)->ref(META_HANDLE);
    SCOPED_TRACE(meta_handle);
    std::map<int64, base::Time>::const_iterator it2 =
        expected_times.find(meta_handle);
    if (it2 == expected_times.end()) {
      ADD_FAILURE() << "Could not find expected time for " << meta_handle;
      continue;
    }
    ExpectTime(**it, it2->second);
  }
}
