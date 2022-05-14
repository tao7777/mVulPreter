bool SyncManager::SyncInternal::Init(
    const FilePath& database_location,
    const WeakHandle<JsEventHandler>& event_handler,
    const std::string& sync_server_and_path,
    int port,
    bool use_ssl,
    HttpPostProviderFactory* post_factory,
    ModelSafeWorkerRegistrar* model_safe_worker_registrar,
    const std::string& user_agent,
    const SyncCredentials& credentials,
    sync_notifier::SyncNotifier* sync_notifier,
    const std::string& restored_key_for_bootstrapping,
    bool setup_for_test_mode) {
  CHECK(!initialized_);

  DCHECK(thread_checker_.CalledOnValidThread());

  VLOG(1) << "Starting SyncInternal initialization.";

  weak_handle_this_ = MakeWeakHandle(weak_ptr_factory_.GetWeakPtr());

  registrar_ = model_safe_worker_registrar;
  setup_for_test_mode_ = setup_for_test_mode;

  sync_notifier_.reset(sync_notifier);

  AddObserver(&js_sync_manager_observer_);
  SetJsEventHandler(event_handler);

  share_.dir_manager.reset(new DirectoryManager(database_location));

  connection_manager_.reset(new SyncAPIServerConnectionManager(
      sync_server_and_path, port, use_ssl, user_agent, post_factory));

  net::NetworkChangeNotifier::AddIPAddressObserver(this);
  observing_ip_address_changes_ = true;
 
   connection_manager()->AddListener(this);
 
       FROM_HERE, base::Bind(&SyncInternal::CheckServerReachable,
                             weak_ptr_factory_.GetWeakPtr()));

  if (!setup_for_test_mode_) {
    VLOG(1) << "Sync is bringing up SyncSessionContext.";
    std::vector<SyncEngineEventListener*> listeners;
    listeners.push_back(&allstatus_);
    listeners.push_back(this);
    SyncSessionContext* context = new SyncSessionContext(
        connection_manager_.get(),
        dir_manager(),
        model_safe_worker_registrar,
        listeners);
    context->set_account_name(credentials.email);
    scheduler_.reset(new SyncScheduler(name_, context, new Syncer()));
  }

  bool signed_in = SignIn(credentials);

  if (signed_in || setup_for_test_mode_) {
    if (scheduler()) {
      scheduler()->Start(
          browser_sync::SyncScheduler::CONFIGURATION_MODE, NULL);
    }

    initialized_ = true;

    BootstrapEncryption(restored_key_for_bootstrapping);
  }

  ObserverList<SyncManager::Observer> temp_obs_list;
  CopyObservers(&temp_obs_list);
  FOR_EACH_OBSERVER(SyncManager::Observer, temp_obs_list,
                    OnInitializationComplete(
                        WeakHandle<JsBackend>(weak_ptr_factory_.GetWeakPtr()),
                        signed_in));

  if (!signed_in && !setup_for_test_mode_)
    return false;

  sync_notifier_->AddObserver(this);

  return signed_in;
}
