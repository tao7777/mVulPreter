void NavigationController::RemoveEntryAtIndex(int index,
void NavigationController::RemoveEntryAtIndex(int index) {
  if (index == last_committed_entry_index_)
    return;

   RemoveEntryAtIndexInternal(index);
 }
