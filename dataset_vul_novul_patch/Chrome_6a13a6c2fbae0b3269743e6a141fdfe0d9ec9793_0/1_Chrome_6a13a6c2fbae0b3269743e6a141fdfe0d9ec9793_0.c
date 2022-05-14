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
 
  if (navigation_entry_index_to_remove_ != -1 && !tab()->is_being_destroyed()) {
    tab()->controller().RemoveEntryAtIndex(navigation_entry_index_to_remove_,
                                           GURL(chrome::kChromeUINewTabURL));
     navigation_entry_index_to_remove_ = -1;
   }
   InterstitialPage::DontProceed();
}
