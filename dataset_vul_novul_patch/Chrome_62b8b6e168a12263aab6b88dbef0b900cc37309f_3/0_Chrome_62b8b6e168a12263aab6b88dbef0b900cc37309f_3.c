void WindowTreeHostManager::OnDisplayAdded(const display::Display& display) {
#if defined(OS_CHROMEOS)
  if (primary_tree_host_for_replace_ &&
      (GetRootWindowSettings(GetWindow(primary_tree_host_for_replace_))
               ->display_id == DisplayManager::kUnifiedDisplayId ||
       display.id() == DisplayManager::kUnifiedDisplayId)) {
    DCHECK_EQ(display::Display::kInvalidDisplayID, primary_display_id);
    primary_display_id = display.id();

    AshWindowTreeHost* ash_host =
        AddWindowTreeHostForDisplay(display, AshWindowTreeHostInitParams());
    RootWindowController::CreateForSecondaryDisplay(ash_host);

    Shell::GetInstance()->magnification_controller()->SwitchTargetRootWindow(
         ash_host->AsWindowTreeHost()->window(), false);
     Shell::GetInstance()
         ->partial_magnification_controller()
        ->SwitchTargetRootWindowIfNeeded(
            ash_host->AsWindowTreeHost()->window());
 
     AshWindowTreeHost* to_delete = primary_tree_host_for_replace_;
     primary_tree_host_for_replace_ = nullptr;

    ash::SystemTray* old_tray =
        GetRootWindowController(to_delete->AsWindowTreeHost()->window())
            ->GetSystemTray();
    ash::SystemTray* new_tray =
        ash::Shell::GetInstance()->GetPrimarySystemTray();
    if (old_tray->GetWidget()->IsVisible()) {
      new_tray->SetVisible(true);
      new_tray->GetWidget()->Show();
    }

    DeleteHost(to_delete);
#ifndef NDEBUG
    auto iter = std::find_if(
        window_tree_hosts_.begin(), window_tree_hosts_.end(),
        [to_delete](const std::pair<int64_t, AshWindowTreeHost*>& pair) {
          return pair.second == to_delete;
        });
    DCHECK(iter == window_tree_hosts_.end());
#endif
  } else
#endif
      if (primary_tree_host_for_replace_) {
    DCHECK(window_tree_hosts_.empty());
    primary_display_id = display.id();
    window_tree_hosts_[display.id()] = primary_tree_host_for_replace_;
    GetRootWindowSettings(GetWindow(primary_tree_host_for_replace_))
        ->display_id = display.id();
    primary_tree_host_for_replace_ = nullptr;
    const DisplayInfo& display_info =
        GetDisplayManager()->GetDisplayInfo(display.id());
    AshWindowTreeHost* ash_host = window_tree_hosts_[display.id()];
    ash_host->AsWindowTreeHost()->SetBounds(display_info.bounds_in_native());
    SetDisplayPropertiesOnHost(ash_host, display);
  } else {
    if (primary_display_id == display::Display::kInvalidDisplayID)
      primary_display_id = display.id();
    DCHECK(!window_tree_hosts_.empty());
    AshWindowTreeHost* ash_host =
        AddWindowTreeHostForDisplay(display, AshWindowTreeHostInitParams());
    RootWindowController::CreateForSecondaryDisplay(ash_host);
  }
}
