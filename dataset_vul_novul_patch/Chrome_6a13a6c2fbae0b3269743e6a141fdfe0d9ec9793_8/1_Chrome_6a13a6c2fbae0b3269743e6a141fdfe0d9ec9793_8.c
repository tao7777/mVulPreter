 void NavigationController::RemoveEntryAtIndexInternal(int index) {
   DCHECK(index < entry_count());
 
   DiscardNonCommittedEntries();
 
   entries_.erase(entries_.begin() + index);
  if (last_committed_entry_index_ == index)
    last_committed_entry_index_--;
  else if (last_committed_entry_index_ > index)
     last_committed_entry_index_--;
 }
