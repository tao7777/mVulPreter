void AppCache::InitializeWithDatabaseRecords(
    const AppCacheDatabase::CacheRecord& cache_record,
    const std::vector<AppCacheDatabase::EntryRecord>& entries,
     const std::vector<AppCacheDatabase::NamespaceRecord>& intercepts,
     const std::vector<AppCacheDatabase::NamespaceRecord>& fallbacks,
     const std::vector<AppCacheDatabase::OnlineWhiteListRecord>& whitelists) {
  DCHECK_EQ(cache_id_, cache_record.cache_id);
   online_whitelist_all_ = cache_record.online_wildcard;
   update_time_ = cache_record.update_time;
 
   for (size_t i = 0; i < entries.size(); ++i) {
     const AppCacheDatabase::EntryRecord& entry = entries.at(i);
     AddEntry(entry.url, AppCacheEntry(entry.flags, entry.response_id,
                                      entry.response_size, entry.padding_size));
   }
  DCHECK_EQ(cache_size_, cache_record.cache_size);
  DCHECK_EQ(padding_size_, cache_record.padding_size);
 
   for (size_t i = 0; i < intercepts.size(); ++i)
     intercept_namespaces_.push_back(intercepts.at(i).namespace_);

  for (size_t i = 0; i < fallbacks.size(); ++i)
    fallback_namespaces_.push_back(fallbacks.at(i).namespace_);

  std::sort(intercept_namespaces_.begin(), intercept_namespaces_.end(),
            SortNamespacesByLength);
  std::sort(fallback_namespaces_.begin(), fallback_namespaces_.end(),
            SortNamespacesByLength);

  for (size_t i = 0; i < whitelists.size(); ++i) {
    const AppCacheDatabase::OnlineWhiteListRecord& record = whitelists.at(i);
    online_whitelist_namespaces_.push_back(
        AppCacheNamespace(APPCACHE_NETWORK_NAMESPACE,
                  record.namespace_url,
                  GURL(),
                  record.is_pattern));
  }
}
