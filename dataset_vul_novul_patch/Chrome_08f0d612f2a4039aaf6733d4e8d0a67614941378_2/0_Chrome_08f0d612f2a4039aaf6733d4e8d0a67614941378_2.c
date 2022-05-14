 void Browser::ProcessPendingTabs() {
  if (!is_attempting_to_close_browser_) {
    // Because we might invoke this after a delay it's possible for the value of
    // is_attempting_to_close_browser_ to have changed since we scheduled the
    // task.
    return;
  }
 
   if (HasCompletedUnloadProcessing()) {
    OnWindowClosing();
    return;
  }

  if (!tabs_needing_before_unload_fired_.empty()) {
    TabContents* tab = *(tabs_needing_before_unload_fired_.begin());
     if (tab->render_view_host()) {
       tab->render_view_host()->FirePageBeforeUnload(false);
     } else {
      ClearUnloadState(tab, true);
     }
   } else if (!tabs_needing_unload_fired_.empty()) {
    TabContents* tab = *(tabs_needing_unload_fired_.begin());
     if (tab->render_view_host()) {
       tab->render_view_host()->ClosePage(false, -1, -1);
     } else {
      ClearUnloadState(tab, true);
     }
   } else {
     NOTREACHED();
  }
}
