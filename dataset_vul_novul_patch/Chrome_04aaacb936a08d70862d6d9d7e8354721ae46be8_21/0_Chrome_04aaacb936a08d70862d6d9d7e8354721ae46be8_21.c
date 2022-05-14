  void StoreExistingGroup() {
    PushNextTask(
        base::BindOnce(&AppCacheStorageImplTest::Verify_StoreExistingGroup,
                       base::Unretained(this)));

     MakeCacheAndGroup(kManifestUrl, 1, 1, true);
    EXPECT_EQ(kDefaultEntrySize + kDefaultEntryPadding,
              storage()->usage_map_[kOrigin]);
 
     cache2_ = new AppCache(storage(), 2);
    cache2_->AddEntry(kEntryUrl, AppCacheEntry(AppCacheEntry::EXPLICIT, 1,
                                               kDefaultEntrySize + 100,
                                               kDefaultEntryPadding + 1000));
 
     storage()->StoreGroupAndNewestCache(group_.get(), cache2_.get(),
                                        delegate());
    EXPECT_FALSE(delegate()->stored_group_success_);
  }
