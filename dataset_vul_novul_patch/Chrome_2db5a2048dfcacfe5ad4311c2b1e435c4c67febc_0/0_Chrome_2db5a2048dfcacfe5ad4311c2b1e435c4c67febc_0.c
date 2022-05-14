bool TabStripModel::InternalCloseTabs(const std::vector<int>& indices,
bool TabStripModel::InternalCloseTabs(const std::vector<int>& in_indices,
                                       uint32 close_types) {
  if (in_indices.empty())
     return true;
 
  std::vector<int> indices(in_indices);
  bool retval = delegate_->CanCloseContents(&indices);
  if (indices.empty())
    return retval;
 
  std::vector<TabContentsWrapper*> tabs;
  for (size_t i = 0; i < indices.size(); ++i)
    tabs.push_back(GetContentsAt(indices[i]));

  if (browser_shutdown::GetShutdownType() == browser_shutdown::NOT_VALID) {
     std::map<RenderProcessHost*, size_t> processes;
     for (size_t i = 0; i < indices.size(); ++i) {
       TabContentsWrapper* detached_contents = GetContentsAt(indices[i]);
       RenderProcessHost* process =
           detached_contents->tab_contents()->GetRenderProcessHost();
      std::map<RenderProcessHost*, size_t>::iterator iter =
          processes.find(process);
      if (iter == processes.end()) {
        processes[process] = 1;
      } else {
        iter->second++;
      }
    }

    for (std::map<RenderProcessHost*, size_t>::iterator iter =
            processes.begin();
        iter != processes.end(); ++iter) {
      iter->first->FastShutdownForPageCount(iter->second);
    }
  }

  for (size_t i = 0; i < tabs.size(); ++i) {
    TabContentsWrapper* detached_contents = tabs[i];
    int index = GetIndexOfTabContents(detached_contents);
    if (index == kNoTab)
      continue;
 
     detached_contents->tab_contents()->OnCloseStarted();
 
    if (!detached_contents->tab_contents()->closed_by_user_gesture()) {
      detached_contents->tab_contents()->set_closed_by_user_gesture(
          close_types & CLOSE_USER_GESTURE);
    }

    if (delegate_->RunUnloadListenerBeforeClosing(detached_contents)) {
      retval = false;
      continue;
    }

    InternalCloseTab(detached_contents, index,
                     (close_types & CLOSE_CREATE_HISTORICAL_TAB) != 0);
  }

  return retval;
}
