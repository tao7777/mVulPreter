 void NavigationControllerImpl::DiscardNonCommittedEntriesInternal() {
   if (pending_entry_index_ == -1)
     delete pending_entry_;
   pending_entry_ = NULL;
   pending_entry_index_ = -1;
  DiscardTransientEntry();
 }
