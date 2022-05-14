void SessionModelAssociator::ReassociateWindows(bool reload_tabs) {
  DCHECK(CalledOnValidThread());
  std::string local_tag = GetCurrentMachineTag();
  sync_pb::SessionSpecifics specifics;
  specifics.set_session_tag(local_tag);
  sync_pb::SessionHeader* header_s = specifics.mutable_header();
  SyncedSession* current_session =
      synced_session_tracker_.GetSession(local_tag);
  header_s->set_client_name(current_session_name_);
#if defined(OS_LINUX)
  header_s->set_device_type(sync_pb::SessionHeader_DeviceType_TYPE_LINUX);
#elif defined(OS_MACOSX)
  header_s->set_device_type(sync_pb::SessionHeader_DeviceType_TYPE_MAC);
#elif defined(OS_WIN)
  header_s->set_device_type(sync_pb::SessionHeader_DeviceType_TYPE_WIN);
#elif defined(OS_CHROMEOS)
  header_s->set_device_type(sync_pb::SessionHeader_DeviceType_TYPE_CROS);
#else
  header_s->set_device_type(sync_pb::SessionHeader_DeviceType_TYPE_OTHER);
#endif

  size_t window_num = 0;
  std::set<SyncedWindowDelegate*> windows =
      SyncedWindowDelegate::GetSyncedWindowDelegates();
  current_session->windows.reserve(windows.size());
  for (std::set<SyncedWindowDelegate*>::const_iterator i =
       windows.begin(); i != windows.end(); ++i) {
    if (ShouldSyncWindow(*i) && (*i)->GetTabCount() &&
        (*i)->HasWindow()) {
      sync_pb::SessionWindow window_s;
      SessionID::id_type window_id = (*i)->GetSessionId();
      VLOG(1) << "Reassociating window " << window_id << " with " <<
          (*i)->GetTabCount() << " tabs.";
      window_s.set_window_id(window_id);
      window_s.set_selected_tab_index((*i)->GetActiveIndex());
      if ((*i)->IsTypeTabbed()) {
        window_s.set_browser_type(
            sync_pb::SessionWindow_BrowserType_TYPE_TABBED);
      } else {
        window_s.set_browser_type(
            sync_pb::SessionWindow_BrowserType_TYPE_POPUP);
      }

      bool found_tabs = false;
      for (int j = 0; j < (*i)->GetTabCount(); ++j) {
        SyncedTabDelegate* tab = (*i)->GetTabAt(j);
        DCHECK(tab);
        if (IsValidTab(*tab)) {
          found_tabs = true;
          window_s.add_tab(tab->GetSessionId());
          if (reload_tabs) {
            ReassociateTab(*tab);
          }
        }
      }
      if (found_tabs) {
        sync_pb::SessionWindow* header_window = header_s->add_window();
        *header_window = window_s;

        if (window_num >= current_session->windows.size()) {
          current_session->windows.push_back(new SessionWindow());
        }
         PopulateSessionWindowFromSpecifics(
             local_tag,
             window_s,
            base::Time::Now(),
             current_session->windows[window_num++],
             &synced_session_tracker_);
       }
    }
  }

  sync_api::WriteTransaction trans(FROM_HERE, sync_service_->GetUserShare());
  sync_api::WriteNode header_node(&trans);
  if (!header_node.InitByIdLookup(local_session_syncid_)) {
    LOG(ERROR) << "Failed to load local session header node.";
    return;
  }
  header_node.SetSessionSpecifics(specifics);
  if (waiting_for_change_) QuitLoopForSubtleTesting();
}
