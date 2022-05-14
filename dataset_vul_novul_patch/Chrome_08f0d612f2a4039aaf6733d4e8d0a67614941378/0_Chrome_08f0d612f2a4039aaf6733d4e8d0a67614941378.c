void Browser::ClearUnloadState(TabContents* tab) {
void Browser::ClearUnloadState(TabContents* tab, bool process_now) {
   if (is_attempting_to_close_browser_) {
     RemoveFromSet(&tabs_needing_before_unload_fired_, tab);
     RemoveFromSet(&tabs_needing_unload_fired_, tab);
    if (process_now) {
      ProcessPendingTabs();
    } else {
      MessageLoop::current()->PostTask(
          FROM_HERE,
          method_factory_.NewRunnableMethod(&Browser::ProcessPendingTabs));
    }
   }
 }
