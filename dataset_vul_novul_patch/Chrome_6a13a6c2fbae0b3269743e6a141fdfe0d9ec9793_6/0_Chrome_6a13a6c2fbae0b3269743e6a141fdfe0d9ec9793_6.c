void NavigationController::NavigateToPendingEntry(ReloadType reload_type) {
  needs_reload_ = false;

  if (pending_entry_index_ != -1 &&
      pending_entry_index_ == last_committed_entry_index_ &&
      (entries_[pending_entry_index_]->restore_type() ==
          NavigationEntry::RESTORE_NONE) &&
       (entries_[pending_entry_index_]->transition_type() &
           content::PAGE_TRANSITION_FORWARD_BACK)) {
     tab_contents_->Stop();

    // If an interstitial page is showing, we want to close it to get back
    // to what was showing before.
    if (tab_contents_->interstitial_page())
      tab_contents_->interstitial_page()->DontProceed();

     DiscardNonCommittedEntries();
     return;
   }
 
  // If an interstitial page is showing, the previous renderer is blocked and
  // cannot make new requests.  Unblock (and disable) it to allow this
  // navigation to succeed.  The interstitial will stay visible until the
  // resulting DidNavigate.
  if (tab_contents_->interstitial_page())
    tab_contents_->interstitial_page()->CancelForNavigation();

   if (!pending_entry_) {
     DCHECK_NE(pending_entry_index_, -1);
    pending_entry_ = entries_[pending_entry_index_].get();
  }

  if (!tab_contents_->NavigateToPendingEntry(reload_type))
    DiscardNonCommittedEntries();

  if (pending_entry_ && !pending_entry_->site_instance() &&
      pending_entry_->restore_type() != NavigationEntry::RESTORE_NONE) {
    pending_entry_->set_site_instance(tab_contents_->GetPendingSiteInstance());
    pending_entry_->set_restore_type(NavigationEntry::RESTORE_NONE);
  }
}
