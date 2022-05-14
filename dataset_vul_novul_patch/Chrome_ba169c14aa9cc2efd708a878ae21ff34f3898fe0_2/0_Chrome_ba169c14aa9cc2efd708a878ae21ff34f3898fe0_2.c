std::unique_ptr<StoragePartitionImpl> StoragePartitionImpl::Create(
    BrowserContext* context,
    bool in_memory,
    const base::FilePath& relative_partition_path,
    const std::string& partition_domain) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI) ||
         !BrowserThread::IsThreadInitialized(BrowserThread::UI));

  base::FilePath partition_path =
      context->GetPath().Append(relative_partition_path);

  std::unique_ptr<StoragePartitionImpl> partition =
      base::WrapUnique(new StoragePartitionImpl(
          context, partition_path, context->GetSpecialStoragePolicy()));

  partition->is_in_memory_ = in_memory;
  partition->relative_partition_path_ = relative_partition_path;

  partition->quota_manager_ = new storage::QuotaManager(
      in_memory, partition_path,
      base::CreateSingleThreadTaskRunnerWithTraits({BrowserThread::IO}).get(),
      context->GetSpecialStoragePolicy(),
      base::BindRepeating(&StoragePartitionImpl::GetQuotaSettings,
                          partition->weak_factory_.GetWeakPtr()));
  scoped_refptr<storage::QuotaManagerProxy> quota_manager_proxy =
      partition->quota_manager_->proxy();

  partition->filesystem_context_ = CreateFileSystemContext(
      context, partition_path, in_memory, quota_manager_proxy.get());

  partition->database_tracker_ = base::MakeRefCounted<storage::DatabaseTracker>(
       partition_path, in_memory, context->GetSpecialStoragePolicy(),
       quota_manager_proxy.get());
 
  partition->dom_storage_context_ = DOMStorageContextWrapper::Create(
       BrowserContext::GetConnectorFor(context),
       in_memory ? base::FilePath() : context->GetPath(),
       relative_partition_path, context->GetSpecialStoragePolicy());

  partition->lock_manager_ = new LockManager();

  base::FilePath path = in_memory ? base::FilePath() : partition_path;
  partition->indexed_db_context_ = new IndexedDBContextImpl(
      path, context->GetSpecialStoragePolicy(), quota_manager_proxy,
      indexed_db::GetDefaultLevelDBFactory());

  partition->cache_storage_context_ = new CacheStorageContextImpl(context);
  partition->cache_storage_context_->Init(path, quota_manager_proxy);

  partition->service_worker_context_ = new ServiceWorkerContextWrapper(context);
  partition->service_worker_context_->set_storage_partition(partition.get());

  partition->appcache_service_ =
      base::MakeRefCounted<ChromeAppCacheService>(quota_manager_proxy.get());

  partition->shared_worker_service_ = std::make_unique<SharedWorkerServiceImpl>(
      partition.get(), partition->service_worker_context_,
      partition->appcache_service_);

  partition->push_messaging_context_ =
      new PushMessagingContext(context, partition->service_worker_context_);

#if !defined(OS_ANDROID)
  partition->host_zoom_level_context_ = new HostZoomLevelContext(
      context->CreateZoomLevelDelegate(partition_path));
#endif  // !defined(OS_ANDROID)

  partition->platform_notification_context_ =
      new PlatformNotificationContextImpl(path, context,
                                          partition->service_worker_context_);
  partition->platform_notification_context_->Initialize();

  partition->background_fetch_context_ =
      base::MakeRefCounted<BackgroundFetchContext>(
          context, partition->service_worker_context_,
          partition->cache_storage_context_, quota_manager_proxy);

  partition->background_sync_context_ =
      base::MakeRefCounted<BackgroundSyncContext>();
  partition->background_sync_context_->Init(partition->service_worker_context_);

  partition->payment_app_context_ = new PaymentAppContextImpl();
  partition->payment_app_context_->Init(partition->service_worker_context_);

  partition->broadcast_channel_provider_ = new BroadcastChannelProvider();

  partition->bluetooth_allowed_devices_map_ = new BluetoothAllowedDevicesMap();

  scoped_refptr<ChromeBlobStorageContext> blob_context =
      ChromeBlobStorageContext::GetFor(context);

  partition->url_loader_factory_getter_ = new URLLoaderFactoryGetter();
  partition->url_loader_factory_getter_->Initialize(partition.get());

  partition->service_worker_context_->Init(
      path, quota_manager_proxy.get(), context->GetSpecialStoragePolicy(),
      blob_context.get(), partition->url_loader_factory_getter_.get());

  partition->blob_registry_ =
      BlobRegistryWrapper::Create(blob_context, partition->filesystem_context_);

  partition->appcache_service_->set_url_loader_factory_getter(
      partition->url_loader_factory_getter_.get());

  partition->prefetch_url_loader_service_ =
      base::MakeRefCounted<PrefetchURLLoaderService>();

  partition->cookie_store_context_ = base::MakeRefCounted<CookieStoreContext>();
  partition->cookie_store_context_->Initialize(
      partition->service_worker_context_, base::DoNothing());

  if (base::FeatureList::IsEnabled(net::features::kIsolatedCodeCache)) {
    GeneratedCodeCacheSettings settings =
        GetContentClient()->browser()->GetGeneratedCodeCacheSettings(context);

    if (!in_memory && settings.enabled()) {
      partition->generated_code_cache_context_ =
          base::MakeRefCounted<GeneratedCodeCacheContext>();

      base::FilePath code_cache_path;
      if (partition_domain.empty()) {
        code_cache_path = settings.path().AppendASCII("Code Cache");
      } else {
        code_cache_path = settings.path()
                              .Append(relative_partition_path)
                              .AppendASCII("Code Cache");
      }
      DCHECK_GE(settings.size_in_bytes(), 0);
      partition->GetGeneratedCodeCacheContext()->Initialize(
          code_cache_path, settings.size_in_bytes());
    }
  }

  return partition;
}
