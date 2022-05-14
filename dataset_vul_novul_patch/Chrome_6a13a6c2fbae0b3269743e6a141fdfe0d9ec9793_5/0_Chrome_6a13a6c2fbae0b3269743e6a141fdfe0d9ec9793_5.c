void NavigationController::InsertOrReplaceEntry(NavigationEntry* entry,
                                                bool replace) {
  DCHECK(entry->transition_type() != content::PAGE_TRANSITION_AUTO_SUBFRAME);

  const NavigationEntry* const pending_entry = (pending_entry_index_ == -1) ?
      pending_entry_ : entries_[pending_entry_index_].get();
  if (pending_entry)
    entry->set_unique_id(pending_entry->unique_id());

  DiscardNonCommittedEntriesInternal();

  int current_size = static_cast<int>(entries_.size());

  if (current_size > 0) {
    if (replace)
      --last_committed_entry_index_;

    int num_pruned = 0;
    while (last_committed_entry_index_ < (current_size - 1)) {
      num_pruned++;
      entries_.pop_back();
      current_size--;
    }
    if (num_pruned > 0)  // Only notify if we did prune something.
      NotifyPrunedEntries(this, false, num_pruned);
   }
 
   if (entries_.size() >= max_entry_count()) {
    DCHECK(last_committed_entry_index_ > 0);
    RemoveEntryAtIndex(0);
     NotifyPrunedEntries(this, true, 1);
   }
 
  entries_.push_back(linked_ptr<NavigationEntry>(entry));
  last_committed_entry_index_ = static_cast<int>(entries_.size()) - 1;

  tab_contents_->UpdateMaxPageID(entry->page_id());
}
