    UserCloudPolicyManagerFactoryChromeOS::CreateManagerForProfile(
        Profile* profile,
        bool force_immediate_load,
        scoped_refptr<base::SequencedTaskRunner> background_task_runner) {
  const CommandLine* command_line = CommandLine::ForCurrentProcess();
  if (chromeos::ProfileHelper::IsSigninProfile(profile))
    return scoped_ptr<UserCloudPolicyManagerChromeOS>();

  chromeos::User* user =
      chromeos::ProfileHelper::Get()->GetUserByProfile(profile);
  CHECK(user);

  // Non-managed domains will be skipped by the below check
   const std::string& username = user->email();
   if (user->GetType() != user_manager::USER_TYPE_REGULAR ||
       BrowserPolicyConnector::IsNonEnterpriseUser(username)) {
    return scoped_ptr<UserCloudPolicyManagerChromeOS>();
  }

   policy::BrowserPolicyConnectorChromeOS* connector =
       g_browser_process->platform_part()->browser_policy_connector_chromeos();
   UserAffiliation affiliation = connector->GetUserAffiliation(username);
  const bool is_affiliated_user = affiliation == USER_AFFILIATION_MANAGED;
   const bool is_browser_restart =
       command_line->HasSwitch(chromeos::switches::kLoginUser);
  const bool wait_for_initial_policy =
      !is_browser_restart &&
      (chromeos::UserManager::Get()->IsCurrentUserNew() || is_affiliated_user);

  const base::TimeDelta initial_policy_fetch_timeout =
      chromeos::UserManager::Get()->IsCurrentUserNew()
          ? base::TimeDelta::Max()
          : base::TimeDelta::FromSeconds(kInitialPolicyFetchTimeoutSeconds);
 
   DeviceManagementService* device_management_service =
       connector->device_management_service();
  if (wait_for_initial_policy)
    device_management_service->ScheduleInitialization(0);

  base::FilePath profile_dir = profile->GetPath();
  const base::FilePath legacy_dir = profile_dir.Append(kDeviceManagementDir);
  const base::FilePath policy_cache_file = legacy_dir.Append(kPolicy);
  const base::FilePath token_cache_file = legacy_dir.Append(kToken);
  const base::FilePath component_policy_cache_dir =
      profile_dir.Append(kPolicy).Append(kComponentsDir);
  const base::FilePath external_data_dir =
        profile_dir.Append(kPolicy).Append(kPolicyExternalDataDir);
  base::FilePath policy_key_dir;
  CHECK(PathService::Get(chromeos::DIR_USER_POLICY_KEYS, &policy_key_dir));

  scoped_ptr<UserCloudPolicyStoreChromeOS> store(
      new UserCloudPolicyStoreChromeOS(
          chromeos::DBusThreadManager::Get()->GetCryptohomeClient(),
          chromeos::DBusThreadManager::Get()->GetSessionManagerClient(),
          background_task_runner,
          username, policy_key_dir, token_cache_file, policy_cache_file));

  scoped_refptr<base::SequencedTaskRunner> backend_task_runner =
      content::BrowserThread::GetBlockingPool()->GetSequencedTaskRunner(
          content::BrowserThread::GetBlockingPool()->GetSequenceToken());
  scoped_refptr<base::SequencedTaskRunner> io_task_runner =
      content::BrowserThread::GetMessageLoopProxyForThread(
          content::BrowserThread::IO);
  scoped_ptr<CloudExternalDataManager> external_data_manager(
      new UserCloudExternalDataManager(base::Bind(&GetChromePolicyDetails),
                                       backend_task_runner,
                                       io_task_runner,
                                       external_data_dir,
                                       store.get()));
  if (force_immediate_load)
    store->LoadImmediately();

  scoped_refptr<base::SequencedTaskRunner> file_task_runner =
      content::BrowserThread::GetMessageLoopProxyForThread(
          content::BrowserThread::FILE);

  scoped_ptr<UserCloudPolicyManagerChromeOS> manager(
      new UserCloudPolicyManagerChromeOS(
          store.PassAs<CloudPolicyStore>(),
           external_data_manager.Pass(),
           component_policy_cache_dir,
           wait_for_initial_policy,
          initial_policy_fetch_timeout,
           base::MessageLoopProxy::current(),
           file_task_runner,
           io_task_runner));

  bool wildcard_match = false;
  if (connector->IsEnterpriseManaged() &&
      chromeos::LoginUtils::IsWhitelisted(username, &wildcard_match) &&
      wildcard_match &&
      !connector->IsNonEnterpriseUser(username)) {
    manager->EnableWildcardLoginCheck(username);
  }

  manager->Init(
      SchemaRegistryServiceFactory::GetForContext(profile)->registry());
  manager->Connect(g_browser_process->local_state(),
                   device_management_service,
                   g_browser_process->system_request_context(),
                   affiliation);

  DCHECK(managers_.find(profile) == managers_.end());
  managers_[profile] = manager.get();
  return manager.Pass();
}
