  void Verify_StoreExistingGroupExistingCache(base::Time expected_update_time) {
    EXPECT_TRUE(delegate()->stored_group_success_);
    EXPECT_EQ(cache_.get(), group_->newest_complete_cache());

    AppCacheDatabase::CacheRecord cache_record;
    EXPECT_TRUE(database()->FindCache(1, &cache_record));
    EXPECT_EQ(1, cache_record.cache_id);
    EXPECT_EQ(1, cache_record.group_id);
     EXPECT_FALSE(cache_record.online_wildcard);
     EXPECT_TRUE(expected_update_time == cache_record.update_time);
     EXPECT_EQ(100 + kDefaultEntrySize, cache_record.cache_size);
 
     std::vector<AppCacheDatabase::EntryRecord> entry_records;
     EXPECT_TRUE(database()->FindEntriesForCache(1, &entry_records));
    EXPECT_EQ(2U, entry_records.size());
    if (entry_records[0].url == kDefaultEntryUrl)
       entry_records.erase(entry_records.begin());
     EXPECT_EQ(1, entry_records[0].cache_id);
     EXPECT_EQ(kEntryUrl, entry_records[0].url);
    EXPECT_EQ(AppCacheEntry::MASTER, entry_records[0].flags);
     EXPECT_EQ(1, entry_records[0].response_id);
     EXPECT_EQ(100, entry_records[0].response_size);
 
    EXPECT_EQ(100 + kDefaultEntrySize, storage()->usage_map_[kOrigin]);
     EXPECT_EQ(1, mock_quota_manager_proxy_->notify_storage_modified_count_);
     EXPECT_EQ(kOrigin, mock_quota_manager_proxy_->last_origin_);
    EXPECT_EQ(100, mock_quota_manager_proxy_->last_delta_);
 
     TestFinished();
   }
