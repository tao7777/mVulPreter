  void Verify_MakeGroupObsolete() {
    EXPECT_TRUE(delegate()->obsoleted_success_);
    EXPECT_EQ(group_.get(), delegate()->obsoleted_group_.get());
    EXPECT_TRUE(group_->is_obsolete());
    EXPECT_TRUE(storage()->usage_map_.empty());

    AppCacheDatabase::GroupRecord group_record;
    AppCacheDatabase::CacheRecord cache_record;
    EXPECT_FALSE(database()->FindGroup(1, &group_record));
    EXPECT_FALSE(database()->FindCache(1, &cache_record));

    std::vector<AppCacheDatabase::EntryRecord> entry_records;
    database()->FindEntriesForCache(1, &entry_records);
    EXPECT_TRUE(entry_records.empty());
    std::vector<AppCacheDatabase::NamespaceRecord> intercept_records;
    std::vector<AppCacheDatabase::NamespaceRecord> fallback_records;
    database()->FindNamespacesForCache(1, &intercept_records,
                                       &fallback_records);
    EXPECT_TRUE(fallback_records.empty());
    std::vector<AppCacheDatabase::OnlineWhiteListRecord> whitelist_records;
    database()->FindOnlineWhiteListForCache(1, &whitelist_records);
    EXPECT_TRUE(whitelist_records.empty());

     EXPECT_TRUE(storage()->usage_map_.empty());
     EXPECT_EQ(1, mock_quota_manager_proxy_->notify_storage_modified_count_);
     EXPECT_EQ(kOrigin, mock_quota_manager_proxy_->last_origin_);
    EXPECT_EQ(-kDefaultEntrySize, mock_quota_manager_proxy_->last_delta_);
 
     TestFinished();
   }
