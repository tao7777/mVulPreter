  void StoreExistingGroupExistingCache() {

     MakeCacheAndGroup(kManifestUrl, 1, 1, true);
    EXPECT_EQ(kDefaultEntrySize, storage()->usage_map_[kOrigin]);
 
     base::Time now = base::Time::Now();
    cache_->AddEntry(kEntryUrl, AppCacheEntry(AppCacheEntry::MASTER, 1, 100));
     cache_->set_update_time(now);
 
     PushNextTask(base::BindOnce(
        &AppCacheStorageImplTest::Verify_StoreExistingGroupExistingCache,
        base::Unretained(this), now));

    EXPECT_EQ(cache_.get(), group_->newest_complete_cache());
    storage()->StoreGroupAndNewestCache(group_.get(), cache_.get(), delegate());
    EXPECT_FALSE(delegate()->stored_group_success_);
  }
