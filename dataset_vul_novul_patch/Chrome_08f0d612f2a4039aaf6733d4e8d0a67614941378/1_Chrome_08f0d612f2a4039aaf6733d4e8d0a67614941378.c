void Browser::ClearUnloadState(TabContents* tab) {
   if (is_attempting_to_close_browser_) {
     RemoveFromSet(&tabs_needing_before_unload_fired_, tab);
     RemoveFromSet(&tabs_needing_unload_fired_, tab);
    ProcessPendingTabs();
   }
 }
