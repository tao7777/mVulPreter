 void TabletModeWindowManager::ArrangeWindowsForClamshellMode(
    base::flat_map<aura::Window*, WindowStateType> windows_in_splitview,
    bool was_in_overview) {
   int divider_position = CalculateCarryOverDividerPostion(windows_in_splitview);
 
   while (window_state_map_.size()) {
     aura::Window* window = window_state_map_.begin()->first;
    ForgetWindow(window, /*destroyed=*/false, was_in_overview);
   }
 
   if (IsClamshellSplitViewModeEnabled()) {
    DoSplitViewTransition(windows_in_splitview, divider_position);
  }
}
