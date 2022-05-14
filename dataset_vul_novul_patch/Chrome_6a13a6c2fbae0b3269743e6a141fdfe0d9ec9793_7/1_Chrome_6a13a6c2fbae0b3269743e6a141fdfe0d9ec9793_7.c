void NavigationController::RemoveEntryAtIndex(int index,
                                              const GURL& default_url) {
  bool is_current = index == last_committed_entry_index_;
   RemoveEntryAtIndexInternal(index);
  if (is_current) {
    if (last_committed_entry_index_ != -1) {
      pending_entry_index_ = last_committed_entry_index_;
      NavigateToPendingEntry(NO_RELOAD);
    } else {
      LoadURL(default_url.is_empty() ? GURL("about:blank") : default_url,
              content::Referrer(), content::PAGE_TRANSITION_START_PAGE,
              std::string());
    }
  }
 }
