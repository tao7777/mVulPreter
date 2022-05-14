bool TabStripModel::InternalCloseTabs(const std::vector<int>& indices,
                                       uint32 close_types) {
  if (indices.empty())
     return true;
 
  bool retval = true;
 
  std::vector<TabContentsWrapper*> tabs;
  for (size_t i = 0; i < indices.size(); ++i)
    tabs.push_back(GetContentsAt(indices[i]));

  if (browser_shutdown::GetShutdownType() == browser_shutdown::NOT_VALID) {
     std::map<RenderProcessHost*, size_t> processes;
     for (size_t i = 0; i < indices.size(); ++i) {
      if (!delegate_->CanCloseContentsAt(indices[i])) {
        retval = false;
        continue;
      }
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
 
    if (!delegate_->CanCloseContentsAt(index)) {
      retval = false;
      continue;
    }
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
