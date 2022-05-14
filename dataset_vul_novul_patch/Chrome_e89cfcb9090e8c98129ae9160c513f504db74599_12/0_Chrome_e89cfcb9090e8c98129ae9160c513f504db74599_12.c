TabContents* TabStripModel::DetachTabContentsAt(int index) {
  if (contents_data_.empty())
    return NULL;

  DCHECK(ContainsIndex(index));

  TabContents* removed_contents = GetTabContentsAtImpl(index);
  bool was_selected = IsTabSelected(index);
  int next_selected_index = order_controller_->DetermineNewSelectedIndex(index);
  delete contents_data_[index];
  contents_data_.erase(contents_data_.begin() + index);
  ForgetOpenersAndGroupsReferencing(removed_contents->web_contents());
   if (empty())
     closing_all_ = true;
   FOR_EACH_OBSERVER(TabStripModelObserver, observers_,
                    TabDetachedAt(removed_contents->web_contents(), index));
   if (empty()) {
     selection_model_.Clear();
    FOR_EACH_OBSERVER(TabStripModelObserver, observers_, TabStripEmpty());
  } else {
    int old_active = active_index();
    selection_model_.DecrementFrom(index);
    TabStripSelectionModel old_model;
    old_model.Copy(selection_model_);
    if (index == old_active) {
      NotifyIfTabDeactivated(removed_contents);
      if (!selection_model_.empty()) {
        selection_model_.set_active(selection_model_.selected_indices()[0]);
        selection_model_.set_anchor(selection_model_.active());
      } else {
        selection_model_.SetSelectedIndex(next_selected_index);
      }
      NotifyIfActiveTabChanged(removed_contents, NOTIFY_DEFAULT);
    }

    if (was_selected) {
      FOR_EACH_OBSERVER(TabStripModelObserver, observers_,
                        TabSelectionChanged(this, old_model));
    }
  }
  return removed_contents;
}
