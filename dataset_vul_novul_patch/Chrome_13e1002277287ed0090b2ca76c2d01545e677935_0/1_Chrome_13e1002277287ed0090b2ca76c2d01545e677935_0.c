 void TabletModeWindowManager::ForgetWindow(aura::Window* window,
                                           bool destroyed) {
   added_windows_.erase(window);
   window->RemoveObserver(this);
 
  WindowToState::iterator it = window_state_map_.find(window);
  if (it == window_state_map_.end())
    return;

  if (destroyed) {
    window_state_map_.erase(it);
   } else {
    it->second->LeaveTabletMode(wm::GetWindowState(it->first));
     DCHECK(!IsTrackingWindow(window));
   }
 }
