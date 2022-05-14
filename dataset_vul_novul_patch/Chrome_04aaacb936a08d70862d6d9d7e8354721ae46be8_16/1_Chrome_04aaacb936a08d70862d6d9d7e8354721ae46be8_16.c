  void SetupMockGroup() {
    std::unique_ptr<net::HttpResponseInfo> info(MakeMockResponseInfo());
    const int kMockInfoSize = GetResponseInfoSize(info.get());

    scoped_refptr<AppCacheGroup> group(
        new AppCacheGroup(service_->storage(), kManifestUrl, kMockGroupId));
    scoped_refptr<AppCache> cache(
        new AppCache(service_->storage(), kMockCacheId));
     cache->AddEntry(
         kManifestUrl,
         AppCacheEntry(AppCacheEntry::MANIFEST, kMockResponseId,
                      kMockInfoSize + kMockBodySize));
     cache->set_complete(true);
     group->AddCache(cache.get());
     mock_storage()->AddStoredGroup(group.get());
    mock_storage()->AddStoredCache(cache.get());
  }
