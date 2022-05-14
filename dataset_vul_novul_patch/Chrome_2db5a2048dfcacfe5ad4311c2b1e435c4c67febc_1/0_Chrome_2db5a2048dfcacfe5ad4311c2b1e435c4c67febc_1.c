bool Browser::CanCloseContentsAt(int index) {
bool Browser::CanCloseContents(std::vector<int>* indices) {
  DCHECK(!indices->empty());
  TabCloseableStateWatcher* watcher =
      g_browser_process->tab_closeable_state_watcher();
  bool can_close_all = !watcher || watcher->CanCloseTabs(this, indices);
  if (indices->empty())  // Cannot close any tab.
     return false;
  // Now, handle cases where at least one tab can be closed.
  // If we are closing all the tabs for this browser, make sure to check for
  if (tab_handler_->GetTabStripModel()->count() ==
          static_cast<int>(indices->size()) &&
      !CanCloseWithInProgressDownloads()) {
    indices->clear();
    can_close_all = false;
  }
  return can_close_all;
 }
