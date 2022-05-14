void SyncBackendHost::Initialize(
    SyncFrontend* frontend,
    const GURL& sync_service_url,
    const syncable::ModelTypeSet& types,
    net::URLRequestContextGetter* baseline_context_getter,
    const SyncCredentials& credentials,
    bool delete_sync_data_folder) {
  if (!core_thread_.Start())
    return;

  frontend_ = frontend;
  DCHECK(frontend);

   registrar_.workers[GROUP_DB] = new DatabaseModelWorker();
   registrar_.workers[GROUP_UI] = new UIModelWorker();
   registrar_.workers[GROUP_PASSIVE] = new ModelSafeWorker();
  registrar_.workers[GROUP_HISTORY] = new HistoryModelWorker(
      profile_->GetHistoryService(Profile::IMPLICIT_ACCESS));
 
  for (syncable::ModelTypeSet::const_iterator it = types.begin();
      it != types.end(); ++it) {
    registrar_.routing_info[(*it)] = GROUP_PASSIVE;
  }

  PasswordStore* password_store =
      profile_->GetPasswordStore(Profile::IMPLICIT_ACCESS);
  if (password_store) {
    registrar_.workers[GROUP_PASSWORD] =
        new PasswordModelWorker(password_store);
  } else {
    LOG_IF(WARNING, types.count(syncable::PASSWORDS) > 0) << "Password store "
        << "not initialized, cannot sync passwords";
    registrar_.routing_info.erase(syncable::PASSWORDS);
  }

  registrar_.routing_info[syncable::NIGORI] = GROUP_PASSIVE;

  core_->CreateSyncNotifier(baseline_context_getter);

  InitCore(Core::DoInitializeOptions(
      sync_service_url,
      MakeHttpBridgeFactory(baseline_context_getter),
      credentials,
      delete_sync_data_folder,
      RestoreEncryptionBootstrapToken(),
      false));
}
