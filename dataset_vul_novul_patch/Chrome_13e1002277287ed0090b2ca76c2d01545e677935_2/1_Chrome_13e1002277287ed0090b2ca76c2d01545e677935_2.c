 void TabletModeWindowManager::Shutdown() {
  base::flat_map<aura::Window*, WindowStateType> windows_in_splitview =
      GetCarryOverWindowsInSplitView();
  SplitViewController* split_view_controller =
      Shell::Get()->split_view_controller();
  if (split_view_controller->InSplitViewMode()) {
    OverviewController* overview_controller =
        Shell::Get()->overview_controller();
    if (!overview_controller->InOverviewSession() ||
        overview_controller->overview_session()->IsEmpty()) {
      Shell::Get()->split_view_controller()->EndSplitView(
          SplitViewController::EndReason::kExitTabletMode);
      overview_controller->EndOverview();
     }
   }
 
  for (aura::Window* window : added_windows_)
    window->RemoveObserver(this);
  added_windows_.clear();
  Shell::Get()->RemoveShellObserver(this);
  Shell::Get()->session_controller()->RemoveObserver(this);
  Shell::Get()->overview_controller()->RemoveObserver(this);
  display::Screen::GetScreen()->RemoveObserver(this);
  RemoveWindowCreationObservers();
 
   ScopedObserveWindowAnimation scoped_observe(GetTopWindow(), this,
                                               /*exiting_tablet_mode=*/true);
  ArrangeWindowsForClamshellMode(windows_in_splitview);
 }
