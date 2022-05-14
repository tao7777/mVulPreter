  void StoreExistingGroup() {
    PushNextTask(
        base::BindOnce(&AppCacheStorageImplTest::Verify_StoreExistingGroup,
                       base::Unretained(this)));

     MakeCacheAndGroup(kManifestUrl, 1, 1, true);
    EXPECT_EQ(kDefaultEntrySize, storage()->usage_map_[kOrigin]);
 
     cache2_ = new AppCache(storage(), 2);
    cache2_->AddEntry(kEntryUrl, AppCacheEntry(AppCacheEntry::MASTER, 1,
                                               kDefaultEntrySize + 100));
 
     storage()->StoreGroupAndNewestCache(group_.get(), cache2_.get(),
                                        delegate());
    EXPECT_FALSE(delegate()->stored_group_success_);
  }
