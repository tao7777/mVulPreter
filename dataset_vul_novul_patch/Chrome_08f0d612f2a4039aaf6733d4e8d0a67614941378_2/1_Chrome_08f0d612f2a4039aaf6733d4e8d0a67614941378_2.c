 void Browser::ProcessPendingTabs() {
  DCHECK(is_attempting_to_close_browser_);
 
   if (HasCompletedUnloadProcessing()) {
    OnWindowClosing();
    return;
  }

  if (!tabs_needing_before_unload_fired_.empty()) {
    TabContents* tab = *(tabs_needing_before_unload_fired_.begin());
     if (tab->render_view_host()) {
       tab->render_view_host()->FirePageBeforeUnload(false);
     } else {
      ClearUnloadState(tab);
     }
   } else if (!tabs_needing_unload_fired_.empty()) {
    TabContents* tab = *(tabs_needing_unload_fired_.begin());
     if (tab->render_view_host()) {
       tab->render_view_host()->ClosePage(false, -1, -1);
     } else {
      ClearUnloadState(tab);
     }
   } else {
     NOTREACHED();
  }
}
