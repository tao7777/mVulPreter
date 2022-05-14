  void Reinitialize(ReinitTestCase test_case) {
    feature_list_.InitAndEnableFeature(network::features::kNetworkService);

    ASSERT_TRUE(temp_directory_.CreateUniqueTempDir());

    AppCacheDatabase db(temp_directory_.GetPath().AppendASCII("Index"));
    EXPECT_TRUE(db.LazyOpen(true));

    if (test_case == CORRUPT_CACHE_ON_INSTALL ||
        test_case == CORRUPT_CACHE_ON_LOAD_EXISTING) {
      const std::string kCorruptData("deadbeef");
      base::FilePath disk_cache_directory =
          temp_directory_.GetPath().AppendASCII("Cache");
      ASSERT_TRUE(base::CreateDirectory(disk_cache_directory));
      base::FilePath index_file = disk_cache_directory.AppendASCII("index");
      EXPECT_EQ(static_cast<int>(kCorruptData.length()),
                base::WriteFile(index_file, kCorruptData.data(),
                                kCorruptData.length()));

      base::FilePath entry_file =
          disk_cache_directory.AppendASCII("01234567_0");
      EXPECT_EQ(static_cast<int>(kCorruptData.length()),
                base::WriteFile(entry_file, kCorruptData.data(),
                                kCorruptData.length()));
    }

    if (test_case == CORRUPT_CACHE_ON_LOAD_EXISTING) {
      AppCacheDatabase db(temp_directory_.GetPath().AppendASCII("Index"));
      GURL manifest_url = GetMockUrl("manifest");

      AppCacheDatabase::GroupRecord group_record;
      group_record.group_id = 1;
      group_record.manifest_url = manifest_url;
      group_record.origin = url::Origin::Create(manifest_url);
      EXPECT_TRUE(db.InsertGroup(&group_record));
      AppCacheDatabase::CacheRecord cache_record;
      cache_record.cache_id = 1;
      cache_record.group_id = 1;
       cache_record.online_wildcard = false;
       cache_record.update_time = kZeroTime;
       cache_record.cache_size = kDefaultEntrySize;
       EXPECT_TRUE(db.InsertCache(&cache_record));
       AppCacheDatabase::EntryRecord entry_record;
       entry_record.cache_id = 1;
       entry_record.url = manifest_url;
       entry_record.flags = AppCacheEntry::MANIFEST;
       entry_record.response_id = 1;
       entry_record.response_size = kDefaultEntrySize;
       EXPECT_TRUE(db.InsertEntry(&entry_record));
     }
 
    service_.reset(new AppCacheServiceImpl(nullptr));
    auto loader_factory_getter = base::MakeRefCounted<URLLoaderFactoryGetter>();
    loader_factory_getter->SetNetworkFactoryForTesting(
        &mock_url_loader_factory_, /* is_corb_enabled = */ true);
    service_->set_url_loader_factory_getter(loader_factory_getter.get());

    service_->Initialize(temp_directory_.GetPath());
    mock_quota_manager_proxy_ = new MockQuotaManagerProxy();
    service_->quota_manager_proxy_ = mock_quota_manager_proxy_;
    delegate_.reset(new MockStorageDelegate(this));

    observer_.reset(new MockServiceObserver(this));
    service_->AddObserver(observer_.get());

    FlushAllTasks();
    base::SequencedTaskRunnerHandle::Get()->PostTask(
        FROM_HERE,
        base::BindOnce(&AppCacheStorageImplTest::Continue_Reinitialize,
                       base::Unretained(this), test_case));
  }
