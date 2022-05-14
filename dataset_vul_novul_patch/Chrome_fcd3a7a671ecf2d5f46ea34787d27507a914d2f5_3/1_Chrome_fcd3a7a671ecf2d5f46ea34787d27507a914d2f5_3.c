void SyncTest::AddOptionalTypesToCommandLine(CommandLine* cl) {
  if (!cl->HasSwitch(switches::kEnableSyncTabs))
    cl->AppendSwitch(switches::kEnableSyncTabs);
}
