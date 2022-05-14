void SafeBrowsingBlockingPage::DontProceed() {
  DCHECK(action_taken() != DONT_PROCEED_ACTION);
  if (action_taken() == PROCEED_ACTION) {
    InterstitialPage::DontProceed();
    return;
  }

  RecordUserAction(DONT_PROCEED);
  FinishMalwareDetails(0);  // No delay

  NotifySafeBrowsingService(sb_service_, unsafe_resources_, false);

  UnsafeResourceMap* unsafe_resource_map = GetUnsafeResourcesMap();
  UnsafeResourceMap::iterator iter = unsafe_resource_map->find(tab());
  if (iter != unsafe_resource_map->end() && !iter->second.empty()) {
    NotifySafeBrowsingService(sb_service_, iter->second, false);
    unsafe_resource_map->erase(iter);
  }
 
  // for the tab has by then already been destroyed.  We also don't delete the
  // current entry if it has been committed again, which is possible on a page
  // that had a subresource warning.
  int last_committed_index = tab()->controller().last_committed_entry_index();
  if (navigation_entry_index_to_remove_ != -1 &&
      navigation_entry_index_to_remove_ != last_committed_index &&
      !tab()->is_being_destroyed()) {
    tab()->controller().RemoveEntryAtIndex(navigation_entry_index_to_remove_);
     navigation_entry_index_to_remove_ = -1;
   }
   InterstitialPage::DontProceed();
}
