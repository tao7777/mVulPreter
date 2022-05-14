bool SyncManager::Init(
    const FilePath& database_location,
    const WeakHandle<JsEventHandler>& event_handler,
    const std::string& sync_server_and_path,
    int sync_server_port,
    bool use_ssl,
    const scoped_refptr<base::TaskRunner>& blocking_task_runner,
    HttpPostProviderFactory* post_factory,
    ModelSafeWorkerRegistrar* registrar,
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
  DCHECK(thread_checker_.CalledOnValidThread());
  DCHECK(post_factory);
  DVLOG(1) << "SyncManager starting Init...";
  std::string server_string(sync_server_and_path);
  return data_->Init(database_location,
                     event_handler,
                     server_string,
                     sync_server_port,
                     use_ssl,
                     blocking_task_runner,
                     post_factory,
                     registrar,
                     extensions_activity_monitor,
                      change_delegate,
                      user_agent,
                      credentials,
                     enable_sync_tabs_for_other_clients,
                      sync_notifier,
                      restored_key_for_bootstrapping,
                      testing_mode,
                     encryptor,
                     unrecoverable_error_handler,
                     report_unrecoverable_error_function);
}
