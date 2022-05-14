  void MakeGroupObsolete() {
    PushNextTask(
        base::BindOnce(&AppCacheStorageImplTest::Verify_MakeGroupObsolete,
                       base::Unretained(this)));

     MakeCacheAndGroup(kManifestUrl, 1, 1, true);
    EXPECT_EQ(kDefaultEntrySize, storage()->usage_map_[kOrigin]);
 
     AppCacheDatabase::EntryRecord entry_record;
    entry_record.cache_id = 1;
    entry_record.flags = AppCacheEntry::FALLBACK;
    entry_record.response_id = 1;
    entry_record.url = kEntryUrl;
    EXPECT_TRUE(database()->InsertEntry(&entry_record));

    AppCacheDatabase::NamespaceRecord fallback_namespace_record;
    fallback_namespace_record.cache_id = 1;
    fallback_namespace_record.namespace_.target_url = kEntryUrl;
    fallback_namespace_record.namespace_.namespace_url = kFallbackNamespace;
    fallback_namespace_record.origin = url::Origin::Create(kManifestUrl);
    EXPECT_TRUE(database()->InsertNamespace(&fallback_namespace_record));

    AppCacheDatabase::OnlineWhiteListRecord online_whitelist_record;
    online_whitelist_record.cache_id = 1;
    online_whitelist_record.namespace_url = kOnlineNamespace;
    EXPECT_TRUE(database()->InsertOnlineWhiteList(&online_whitelist_record));

    storage()->MakeGroupObsolete(group_.get(), delegate(), 0);
    EXPECT_FALSE(group_->is_obsolete());
  }
