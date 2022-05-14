  void MakeCacheAndGroup(const GURL& manifest_url,
                         int64_t group_id,
                         int64_t cache_id,
                          bool add_to_database) {
     AppCacheEntry default_entry(AppCacheEntry::EXPLICIT,
                                 cache_id + kDefaultEntryIdOffset,
                                kDefaultEntrySize, kDefaultEntryPadding);
     group_ = new AppCacheGroup(storage(), manifest_url, group_id);
     cache_ = new AppCache(storage(), cache_id);
     cache_->AddEntry(kDefaultEntryUrl, default_entry);
    cache_->set_complete(true);
    group_->AddCache(cache_.get());
    url::Origin manifest_origin(url::Origin::Create(manifest_url));
    if (add_to_database) {
      AppCacheDatabase::GroupRecord group_record;
      group_record.group_id = group_id;
      group_record.manifest_url = manifest_url;
      group_record.origin = manifest_origin;
      EXPECT_TRUE(database()->InsertGroup(&group_record));
      AppCacheDatabase::CacheRecord cache_record;
      cache_record.cache_id = cache_id;
      cache_record.group_id = group_id;
       cache_record.online_wildcard = false;
       cache_record.update_time = kZeroTime;
       cache_record.cache_size = kDefaultEntrySize;
      cache_record.padding_size = kDefaultEntryPadding;
       EXPECT_TRUE(database()->InsertCache(&cache_record));
       AppCacheDatabase::EntryRecord entry_record;
       entry_record.cache_id = cache_id;
       entry_record.url = kDefaultEntryUrl;
       entry_record.flags = default_entry.types();
       entry_record.response_id = default_entry.response_id();
       entry_record.response_size = default_entry.response_size();
      entry_record.padding_size = default_entry.padding_size();
       EXPECT_TRUE(database()->InsertEntry(&entry_record));
 
      storage()->usage_map_[manifest_origin] =
          default_entry.response_size() + default_entry.padding_size();
     }
   }
