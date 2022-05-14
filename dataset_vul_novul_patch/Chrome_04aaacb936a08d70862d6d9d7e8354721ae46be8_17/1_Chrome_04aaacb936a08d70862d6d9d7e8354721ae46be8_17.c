  void FailStoreGroup() {
     PushNextTask(base::BindOnce(&AppCacheStorageImplTest::Verify_FailStoreGroup,
                                 base::Unretained(this)));
 
     const int64_t kTooBig = 10 * 1024 * 1024;  // 10M
     group_ =
         new AppCacheGroup(storage(), kManifestUrl, storage()->NewGroupId());
     cache_ = new AppCache(storage(), storage()->NewCacheId());
    cache_->AddEntry(kManifestUrl,
                     AppCacheEntry(AppCacheEntry::MANIFEST, 1, kTooBig));
 
    storage()->StoreGroupAndNewestCache(group_.get(), cache_.get(), delegate());
    EXPECT_FALSE(delegate()->stored_group_success_);  // Expected to be async.
  }
