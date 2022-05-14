void AppCache::ToDatabaseRecords(
    const AppCacheGroup* group,
    AppCacheDatabase::CacheRecord* cache_record,
    std::vector<AppCacheDatabase::EntryRecord>* entries,
    std::vector<AppCacheDatabase::NamespaceRecord>* intercepts,
    std::vector<AppCacheDatabase::NamespaceRecord>* fallbacks,
    std::vector<AppCacheDatabase::OnlineWhiteListRecord>* whitelists) {
  DCHECK(group && cache_record && entries && fallbacks && whitelists);
  DCHECK(entries->empty() && fallbacks->empty() && whitelists->empty());

  cache_record->cache_id = cache_id_;
  cache_record->group_id = group->group_id();
   cache_record->online_wildcard = online_whitelist_all_;
   cache_record->update_time = update_time_;
   cache_record->cache_size = cache_size_;
 
   for (const auto& pair : entries_) {
     entries->push_back(AppCacheDatabase::EntryRecord());
    AppCacheDatabase::EntryRecord& record = entries->back();
    record.url = pair.first;
    record.cache_id = cache_id_;
     record.flags = pair.second.types();
     record.response_id = pair.second.response_id();
     record.response_size = pair.second.response_size();
   }
 
   const url::Origin origin = url::Origin::Create(group->manifest_url());

  for (size_t i = 0; i < intercept_namespaces_.size(); ++i) {
    intercepts->push_back(AppCacheDatabase::NamespaceRecord());
    AppCacheDatabase::NamespaceRecord& record = intercepts->back();
    record.cache_id = cache_id_;
    record.origin = origin;
    record.namespace_ = intercept_namespaces_[i];
  }

  for (size_t i = 0; i < fallback_namespaces_.size(); ++i) {
    fallbacks->push_back(AppCacheDatabase::NamespaceRecord());
    AppCacheDatabase::NamespaceRecord& record = fallbacks->back();
    record.cache_id = cache_id_;
    record.origin = origin;
    record.namespace_ = fallback_namespaces_[i];
  }

  for (size_t i = 0; i < online_whitelist_namespaces_.size(); ++i) {
    whitelists->push_back(AppCacheDatabase::OnlineWhiteListRecord());
    AppCacheDatabase::OnlineWhiteListRecord& record = whitelists->back();
    record.cache_id = cache_id_;
    record.namespace_url = online_whitelist_namespaces_[i].namespace_url;
    record.is_pattern = online_whitelist_namespaces_[i].is_pattern;
  }
}
