  void StoreNewGroup() {
    PushNextTask(base::BindOnce(&AppCacheStorageImplTest::Verify_StoreNewGroup,
                                base::Unretained(this)));

     group_ =
         new AppCacheGroup(storage(), kManifestUrl, storage()->NewGroupId());
     cache_ = new AppCache(storage(), storage()->NewCacheId());
    cache_->AddEntry(kEntryUrl,
                     AppCacheEntry(AppCacheEntry::MASTER, 1, kDefaultEntrySize,
                                   /*padding_size=*/0));
 
    mock_quota_manager_proxy_->mock_manager_->async_ = true;

    storage()->StoreGroupAndNewestCache(group_.get(), cache_.get(), delegate());
    EXPECT_FALSE(delegate()->stored_group_success_);
  }
