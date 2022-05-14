  void Verify_StoreExistingGroup() {
    EXPECT_TRUE(delegate()->stored_group_success_);
    EXPECT_EQ(group_.get(), delegate()->stored_group_.get());
    EXPECT_EQ(cache2_.get(), group_->newest_complete_cache());
    EXPECT_TRUE(cache2_->is_complete());

    AppCacheDatabase::GroupRecord group_record;
    AppCacheDatabase::CacheRecord cache_record;
    EXPECT_TRUE(database()->FindGroup(1, &group_record));
    EXPECT_TRUE(database()->FindCache(2, &cache_record));

     EXPECT_FALSE(database()->FindCache(1, &cache_record));
 
    EXPECT_EQ(kDefaultEntrySize + 100, storage()->usage_map_[kOrigin]);
     EXPECT_EQ(1, mock_quota_manager_proxy_->notify_storage_modified_count_);
     EXPECT_EQ(kOrigin, mock_quota_manager_proxy_->last_origin_);
    EXPECT_EQ(100, mock_quota_manager_proxy_->last_delta_);
 
     TestFinished();
   }
