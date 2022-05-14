 void SessionModelAssociator::PopulateSessionWindowFromSpecifics(
     const std::string& session_tag,
     const sync_pb::SessionWindow& specifics,
    const base::Time& mtime,
     SessionWindow* session_window,
     SyncedSessionTracker* tracker) {
   if (specifics.has_window_id())
    session_window->window_id.set_id(specifics.window_id());
  if (specifics.has_selected_tab_index())
    session_window->selected_tab_index = specifics.selected_tab_index();
  if (specifics.has_browser_type()) {
    if (specifics.browser_type() ==
        sync_pb::SessionWindow_BrowserType_TYPE_TABBED) {
      session_window->type = 1;
    } else {
       session_window->type = 2;
     }
   }
  session_window->timestamp = mtime;
   session_window->tabs.resize(specifics.tab_size());
   for (int i = 0; i < specifics.tab_size(); i++) {
     SessionID::id_type tab_id = specifics.tab(i);
    session_window->tabs[i] =
        tracker->GetSessionTab(session_tag, tab_id, true);
  }
}
