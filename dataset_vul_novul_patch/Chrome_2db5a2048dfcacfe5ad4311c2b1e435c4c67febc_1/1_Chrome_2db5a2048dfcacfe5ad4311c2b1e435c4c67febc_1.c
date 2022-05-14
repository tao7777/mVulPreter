bool Browser::CanCloseContentsAt(int index) {
  if (!CanCloseTab())
     return false;
  if (tab_handler_->GetTabStripModel()->count() > 1)
    return true;
  return CanCloseWithInProgressDownloads();
 }
