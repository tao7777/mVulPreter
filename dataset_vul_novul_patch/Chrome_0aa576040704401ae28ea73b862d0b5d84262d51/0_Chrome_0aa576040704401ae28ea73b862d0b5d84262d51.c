void NavigatorImpl::DiscardPendingEntryIfNeeded(int expected_pending_entry_id) {
  NavigationEntry* pending_entry = controller_->GetPendingEntry();
  bool pending_matches_fail_msg =
      pending_entry &&
      expected_pending_entry_id == pending_entry->GetUniqueID();
  if (!pending_matches_fail_msg)
    return;

  // Do not leave the pending entry visible if it has an invalid URL, since this
  // might be formatted in an unexpected or unsafe way.
  // TODO(creis): Block navigations to invalid URLs in https://crbug.com/850824.
  //
  bool should_preserve_entry = pending_entry->GetURL().is_valid() &&
                               (controller_->IsUnmodifiedBlankTab() ||
                                delegate_->ShouldPreserveAbortedURLs());
   if (pending_entry != controller_->GetVisibleEntry() ||
       !should_preserve_entry) {
     controller_->DiscardPendingEntry(true);

    controller_->delegate()->NotifyNavigationStateChanged(INVALIDATE_TYPE_URL);
  }
}
