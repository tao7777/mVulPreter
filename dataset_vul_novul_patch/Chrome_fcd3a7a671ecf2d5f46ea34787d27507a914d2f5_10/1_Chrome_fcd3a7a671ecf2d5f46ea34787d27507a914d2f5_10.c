bool SyncManager::ReceivedExperiment(browser_sync::Experiments* experiments)
    const {
  ReadTransaction trans(FROM_HERE, GetUserShare());
  ReadNode node(&trans);
  if (node.InitByTagLookup(kNigoriTag) != sync_api::BaseNode::INIT_OK) {
    DVLOG(1) << "Couldn't find Nigori node.";
     return false;
   }
   bool found_experiment = false;
  if (node.GetNigoriSpecifics().sync_tabs()) {
    experiments->sync_tabs = true;
    found_experiment = true;
  }
   if (node.GetNigoriSpecifics().sync_tab_favicons()) {
     experiments->sync_tab_favicons = true;
     found_experiment = true;
  }
  return found_experiment;
}
