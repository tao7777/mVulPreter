bool SyncManager::SyncInternal::Init(
    const FilePath& database_location,
    const WeakHandle<JsEventHandler>& event_handler,
    const std::string& sync_server_and_path,
    int port,
    bool use_ssl,
    const scoped_refptr<base::TaskRunner>& blocking_task_runner,
    HttpPostProviderFactory* post_factory,
    ModelSafeWorkerRegistrar* model_safe_worker_registrar,
    browser_sync::ExtensionsActivityMonitor* extensions_activity_monitor,
     ChangeDelegate* change_delegate,
     const std::string& user_agent,
     const SyncCredentials& credentials,
    bool enable_sync_tabs_for_other_clients,
     sync_notifier::SyncNotifier* sync_notifier,
     const std::string& restored_key_for_bootstrapping,
     TestingMode testing_mode,
    Encryptor* encryptor,
    UnrecoverableErrorHandler* unrecoverable_error_handler,
    ReportUnrecoverableErrorFunction report_unrecoverable_error_function) {
  CHECK(!initialized_);

  DCHECK(thread_checker_.CalledOnValidThread());

  DVLOG(1) << "Starting SyncInternal initialization.";

  weak_handle_this_ = MakeWeakHandle(weak_ptr_factory_.GetWeakPtr());

  blocking_task_runner_ = blocking_task_runner;

  registrar_ = model_safe_worker_registrar;
   change_delegate_ = change_delegate;
   testing_mode_ = testing_mode;
 
  enable_sync_tabs_for_other_clients_ = enable_sync_tabs_for_other_clients;
   sync_notifier_.reset(sync_notifier);
 
   AddObserver(&js_sync_manager_observer_);
  SetJsEventHandler(event_handler);

  AddObserver(&debug_info_event_listener_);

  database_path_ = database_location.Append(
      syncable::Directory::kSyncDatabaseFilename);
  encryptor_ = encryptor;
  unrecoverable_error_handler_ = unrecoverable_error_handler;
  report_unrecoverable_error_function_ = report_unrecoverable_error_function;
  share_.directory.reset(
      new syncable::Directory(encryptor_,
                              unrecoverable_error_handler_,
                              report_unrecoverable_error_function_));

  connection_manager_.reset(new SyncAPIServerConnectionManager(
      sync_server_and_path, port, use_ssl, user_agent, post_factory));

  net::NetworkChangeNotifier::AddIPAddressObserver(this);
  observing_ip_address_changes_ = true;

  connection_manager()->AddListener(this);


  if (testing_mode_ == NON_TEST) {
    DVLOG(1) << "Sync is bringing up SyncSessionContext.";
    std::vector<SyncEngineEventListener*> listeners;
    listeners.push_back(&allstatus_);
    listeners.push_back(this);
    SyncSessionContext* context = new SyncSessionContext(
        connection_manager_.get(),
        directory(),
        model_safe_worker_registrar,
        extensions_activity_monitor,
        listeners,
        &debug_info_event_listener_,
        &traffic_recorder_);
    context->set_account_name(credentials.email);
    scheduler_.reset(new SyncScheduler(name_, context, new Syncer()));
  }

  bool signed_in = SignIn(credentials);

  if (signed_in) {
    if (scheduler()) {
      scheduler()->Start(
          browser_sync::SyncScheduler::CONFIGURATION_MODE, base::Closure());
    }

    initialized_ = true;

    ReadTransaction trans(FROM_HERE, GetUserShare());
    trans.GetCryptographer()->Bootstrap(restored_key_for_bootstrapping);
    trans.GetCryptographer()->AddObserver(this);
  }

  FOR_EACH_OBSERVER(SyncManager::Observer, observers_,
                    OnInitializationComplete(
                        MakeWeakHandle(weak_ptr_factory_.GetWeakPtr()),
                        signed_in));

  if (!signed_in && testing_mode_ == NON_TEST)
    return false;

  sync_notifier_->AddObserver(this);

  return signed_in;
}
