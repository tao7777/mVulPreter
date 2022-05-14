  void FailStoreGroup() {
  void FailStoreGroup_SizeTooBig() {
     PushNextTask(base::BindOnce(&AppCacheStorageImplTest::Verify_FailStoreGroup,
                                 base::Unretained(this)));
 
    // Set up some preconditions. Create a group and newest cache that
     const int64_t kTooBig = 10 * 1024 * 1024;  // 10M
     group_ =
         new AppCacheGroup(storage(), kManifestUrl, storage()->NewGroupId());
     cache_ = new AppCache(storage(), storage()->NewCacheId());
    cache_->AddEntry(kManifestUrl, AppCacheEntry(AppCacheEntry::MANIFEST,
                                                 /*response_id=*/1,
                                                 /*response_size=*/kTooBig,
                                                 /*padding_size=*/0));
    // Hold a ref to the cache to simulate the UpdateJob holding that ref,
    // and hold a ref to the group to simulate the CacheHost holding that ref.

    // Conduct the store test.
    storage()->StoreGroupAndNewestCache(group_.get(), cache_.get(), delegate());
    EXPECT_FALSE(delegate()->stored_group_success_);  // Expected to be async.
  }

  void FailStoreGroup_PaddingTooBig() {
    // Store a group and its newest cache. Should complete asynchronously.
    PushNextTask(base::BindOnce(&AppCacheStorageImplTest::Verify_FailStoreGroup,
                                base::Unretained(this)));

    // Set up some preconditions. Create a group and newest cache that
    // appear to be "unstored" and big enough to exceed the 5M limit.
    const int64_t kTooBig = 10 * 1024 * 1024;  // 10M
    group_ =
        new AppCacheGroup(storage(), kManifestUrl, storage()->NewGroupId());
    cache_ = new AppCache(storage(), storage()->NewCacheId());
    cache_->AddEntry(kEntryUrl, AppCacheEntry(AppCacheEntry::EXPLICIT,
                                              /*response_id=*/1,
                                              /*response_size=*/1,
                                              /*padding_size=*/kTooBig));
    // Hold a ref to the cache to simulate the UpdateJob holding that ref,
 
    storage()->StoreGroupAndNewestCache(group_.get(), cache_.get(), delegate());
    EXPECT_FALSE(delegate()->stored_group_success_);  // Expected to be async.
  }
