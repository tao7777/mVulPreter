 void TabletModeWindowManager::Shutdown() {
  base::flat_map<aura::Window*, WindowStateType> carryover_windows_in_splitview;
  const bool was_in_overview =
      Shell::Get()->overview_controller()->InOverviewSession();
  // If clamshell split view mode is not enabled, still keep the old behavior:
  // End overview if overview is active and restore all windows' window states
  // to their previous window states.
  if (!IsClamshellSplitViewModeEnabled()) {
    Shell::Get()->overview_controller()->EndOverview();
  } else {
    // If clamshell split view mode is enabled, there are 4 cases when exiting
    // tablet mode:
    // 1) overview is active but split view is inactive: keep overview active in
    //    clamshell mode.
    // 2) overview and splitview are both active: keep overview and splitview
    // both
    //    active in clamshell mode, unless if it's single split state, splitview
    //    and overview will both be ended.
    // 3) overview is inactive but split view is active (two snapped windows):
    //    split view is no longer active. But the two snapped windows will still
    //    keep snapped in clamshell mode.
    // 4) overview and splitview are both inactive: keep the current behavior,
    //    i.e., restore all windows to its window state before entering tablet
    //    mode.

    // TODO(xdai): Instead of caching snapped windows and their state here, we
    // should try to see if it can be done in the WindowState::State impl.
    carryover_windows_in_splitview = GetCarryOverWindowsInSplitView();

    // For case 2 and 3: End splitview mode for two snapped windows case or
    // single split case to match the clamshell split view behavior. (there is
    // no both snapped state or single split state in clamshell split view). The
    // windows will still be kept snapped though.
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
  ArrangeWindowsForClamshellMode(carryover_windows_in_splitview,
                                 was_in_overview);
 }
