std::unique_ptr<PrefService> CreatePrefService() {
  auto pref_registry = base::MakeRefCounted<user_prefs::PrefRegistrySyncable>();
 
   metrics::MetricsService::RegisterPrefs(pref_registry.get());
   variations::VariationsService::RegisterPrefs(pref_registry.get());
  pref_registry->RegisterIntegerPref(prefs::kRestartsWithStaleSeed, 0);
 
   AwBrowserProcess::RegisterNetworkContextLocalStatePrefs(pref_registry.get());
 
  PrefServiceFactory pref_service_factory;

  std::set<std::string> persistent_prefs;
  for (const char* const pref_name : kPersistentPrefsWhitelist)
    persistent_prefs.insert(pref_name);

  pref_service_factory.set_user_prefs(base::MakeRefCounted<SegregatedPrefStore>(
      base::MakeRefCounted<InMemoryPrefStore>(),
      base::MakeRefCounted<JsonPrefStore>(GetPrefStorePath()), persistent_prefs,
      mojo::Remote<::prefs::mojom::TrackedPreferenceValidationDelegate>()));

  pref_service_factory.set_read_error_callback(
      base::BindRepeating(&HandleReadError));

   return pref_service_factory.Create(pref_registry);
 }
