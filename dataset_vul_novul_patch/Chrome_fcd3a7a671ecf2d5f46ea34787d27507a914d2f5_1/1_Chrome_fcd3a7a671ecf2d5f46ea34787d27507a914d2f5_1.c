void ProfileSyncService::OnExperimentsChanged(
    const browser_sync::Experiments& experiments) {
  if (current_experiments.Matches(experiments))
    return;

  if (migrator_.get() &&
      migrator_->state() != browser_sync::BackendMigrator::IDLE) {
    DVLOG(1) << "Dropping OnExperimentsChanged due to migrator busy.";
    return;
  }
 
   const syncable::ModelTypeSet registered_types = GetRegisteredDataTypes();
   syncable::ModelTypeSet to_add;
  if (experiments.sync_tabs)
    to_add.Put(syncable::SESSIONS);
   const syncable::ModelTypeSet to_register =
       Difference(to_add, registered_types);
   DVLOG(2) << "OnExperimentsChanged called with types: "
           << syncable::ModelTypeSetToString(to_add);
  DVLOG(2) << "Enabling types: " << syncable::ModelTypeSetToString(to_register);

  for (syncable::ModelTypeSet::Iterator it = to_register.First();
       it.Good(); it.Inc()) {
    RegisterNewDataType(it.Get());
#if !defined(OS_ANDROID)
    std::string experiment_name = GetExperimentNameForDataType(it.Get());
    if (experiment_name.empty())
      continue;
    about_flags::SetExperimentEnabled(g_browser_process->local_state(),
                                      experiment_name,
                                      true);
#endif  // !defined(OS_ANDROID)
  }

  if (sync_prefs_.HasKeepEverythingSynced()) {
    sync_prefs_.SetPreferredDataTypes(registered_types, registered_types);

    if (!to_register.Empty() && HasSyncSetupCompleted() && migrator_.get()) {
      DVLOG(1) << "Dynamically enabling new datatypes: "
               << syncable::ModelTypeSetToString(to_register);
      OnMigrationNeededForTypes(to_register);
    }
  }

  if (experiments.sync_tab_favicons) {
    DVLOG(1) << "Enabling syncing of tab favicons.";
    about_flags::SetExperimentEnabled(g_browser_process->local_state(),
                                      "sync-tab-favicons",
                                      true);
  }

  current_experiments = experiments;
}
