 void TabletModeWindowManager::Init() {
  // If clamshell split view mode is not enabled, still keep the old behavior:
  // end overview if it's active. And carry over snapped windows to splitscreen
  // if possible.
  if (!IsClamshellSplitViewModeEnabled())
    Shell::Get()->overview_controller()->EndOverview();

  // If clamshell splitview mode is enabled, there are 3 cases when entering
  // tablet mode:

  {
    ScopedObserveWindowAnimation scoped_observe(GetTopWindow(), this,
                                                /*exiting_tablet_mode=*/false);
    ArrangeWindowsForTabletMode();
  }
  AddWindowCreationObservers();
  display::Screen::GetScreen()->AddObserver(this);
  Shell::Get()->AddShellObserver(this);
  Shell::Get()->session_controller()->AddObserver(this);
  Shell::Get()->overview_controller()->AddObserver(this);
  accounts_since_entering_tablet_.insert(
      Shell::Get()->session_controller()->GetActiveAccountId());
  event_handler_ = std::make_unique<wm::TabletModeEventHandler>();
 }
