void BrowserWindowGtk::UpdateDevToolsForContents(WebContents* contents) {
  TRACE_EVENT0("ui::gtk", "BrowserWindowGtk::UpdateDevToolsForContents");
  DevToolsWindow* new_devtools_window = contents ?
      DevToolsWindow::GetDockedInstanceForInspectedTab(contents) : NULL;

  if (devtools_window_ == new_devtools_window && (!new_devtools_window ||
        new_devtools_window->dock_side() == devtools_dock_side_))
    return;
 
   if (devtools_window_ != new_devtools_window) {
    if (devtools_window_) {
      devtools_container_->DetachTab(
          devtools_window_->tab_contents()->web_contents());
    }
     devtools_container_->SetTab(
         new_devtools_window ? new_devtools_window->tab_contents() : NULL);
     if (new_devtools_window) {
      new_devtools_window->tab_contents()->web_contents()->WasShown();
    }
  }

  if (devtools_window_) {
    GtkAllocation contents_rect;
    gtk_widget_get_allocation(contents_vsplit_, &contents_rect);
    if (devtools_dock_side_ == DEVTOOLS_DOCK_SIDE_RIGHT) {
      devtools_window_->SetWidth(
          contents_rect.width -
          gtk_paned_get_position(GTK_PANED(contents_hsplit_)));
    } else {
      devtools_window_->SetHeight(
          contents_rect.height -
          gtk_paned_get_position(GTK_PANED(contents_vsplit_)));
    }
  }

  bool should_hide = devtools_window_ && (!new_devtools_window ||
      devtools_dock_side_ != new_devtools_window->dock_side());
  bool should_show = new_devtools_window && (!devtools_window_ || should_hide);

  if (should_hide)
    HideDevToolsContainer();

  devtools_window_ = new_devtools_window;

  if (should_show) {
    devtools_dock_side_ = new_devtools_window->dock_side();
    ShowDevToolsContainer();
  } else if (new_devtools_window) {
    UpdateDevToolsSplitPosition();
  }
}
