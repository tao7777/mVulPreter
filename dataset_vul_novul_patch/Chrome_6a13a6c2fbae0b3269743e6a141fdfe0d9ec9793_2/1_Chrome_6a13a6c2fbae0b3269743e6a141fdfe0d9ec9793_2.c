void NavigationController::GoBack() {
  if (!CanGoBack()) {
    NOTREACHED();
     return;
   }
 
  if (tab_contents_->interstitial_page()) {
    tab_contents_->interstitial_page()->DontProceed();
    return;
  }
   int current_index = GetCurrentEntryIndex();
 
  DiscardNonCommittedEntries();

  pending_entry_index_ = current_index - 1;
  entries_[pending_entry_index_]->set_transition_type(
      content::PageTransitionFromInt(
          entries_[pending_entry_index_]->transition_type() |
          content::PAGE_TRANSITION_FORWARD_BACK));
  NavigateToPendingEntry(NO_RELOAD);
}
