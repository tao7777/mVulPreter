int64 ClientUsageTracker::GetCachedHostUsage(const std::string& host) {
int64 ClientUsageTracker::GetCachedHostUsage(const std::string& host) const {
   HostUsageMap::const_iterator found = cached_usage_.find(host);
   if (found == cached_usage_.end())
     return 0;

  int64 usage = 0;
  const UsageMap& map = found->second;
  for (UsageMap::const_iterator iter = map.begin();
       iter != map.end(); ++iter) {
    usage += iter->second;
  }
  return usage;
}
