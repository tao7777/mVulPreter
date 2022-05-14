void SyncBackendHost::Core::DoInitialize(const DoInitializeOptions& options) {
  DCHECK(!sync_loop_);
  sync_loop_ = options.sync_loop;
  DCHECK(sync_loop_);

  if (options.delete_sync_data_folder) {
    DeleteSyncDataFolder();
  }

  bool success = file_util::CreateDirectory(sync_data_folder_path_);
  DCHECK(success);

  DCHECK(!registrar_);
  registrar_ = options.registrar;
  DCHECK(registrar_);

  sync_manager_.reset(new sync_api::SyncManager(name_));
  sync_manager_->AddObserver(this);
  success = sync_manager_->Init(
      sync_data_folder_path_,
      options.event_handler,
      options.service_url.host() + options.service_url.path(),
      options.service_url.EffectiveIntPort(),
      options.service_url.SchemeIsSecure(),
      BrowserThread::GetBlockingPool(),
      options.make_http_bridge_factory_fn.Run(),
      options.registrar /* as ModelSafeWorkerRegistrar */,
      options.extensions_activity_monitor,
       options.registrar /* as SyncManager::ChangeDelegate */,
       MakeUserAgentForSyncApi(),
       options.credentials,
       new BridgedSyncNotifier(
           options.chrome_sync_notification_bridge,
           options.sync_notifier_factory->CreateSyncNotifier()),
      options.restored_key_for_bootstrapping,
      options.testing_mode,
      &encryptor_,
      options.unrecoverable_error_handler,
      options.report_unrecoverable_error_function);
  LOG_IF(ERROR, !success) << "Syncapi initialization failed!";

  if (CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kSyncThrowUnrecoverableError)) {
    sync_manager_->ThrowUnrecoverableError();
  }
}
