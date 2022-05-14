bool SessionModelAssociator::WriteTabContentsToSyncModel(
    const SyncedWindowDelegate& window,
    const SyncedTabDelegate& tab,
    int64 sync_id,
    sync_api::WriteTransaction* trans) {

  DCHECK(CalledOnValidThread());
  sync_api::WriteNode tab_node(trans);
  if (!tab_node.InitByIdLookup(sync_id)) {
    LOG(ERROR) << "Failed to look up tab node " << sync_id;
    return false;
  }

  sync_pb::SessionSpecifics session_s;
  session_s.set_session_tag(GetCurrentMachineTag());
  sync_pb::SessionTab* tab_s = session_s.mutable_tab();

  SessionID::id_type tab_id = tab.GetSessionId();
  tab_s->set_tab_id(tab_id);
  tab_s->set_window_id(tab.GetWindowId());
  const int current_index = tab.GetCurrentEntryIndex();
  const int min_index = std::max(0,
                                 current_index - max_sync_navigation_count);
  const int max_index = std::min(current_index + max_sync_navigation_count,
                                 tab.GetEntryCount());
  const int pending_index = tab.GetPendingEntryIndex();
  tab_s->set_pinned(window.IsTabPinned(&tab));
  if (tab.HasExtensionAppId()) {
    tab_s->set_extension_app_id(tab.GetExtensionAppId());
  }
  for (int i = min_index; i < max_index; ++i) {
    const NavigationEntry* entry = (i == pending_index) ?
       tab.GetPendingEntry() : tab.GetEntryAtIndex(i);
    DCHECK(entry);
    if (entry->virtual_url().is_valid()) {
      if (i == max_index - 1) {
        VLOG(1) << "Associating tab " << tab_id << " with sync id " << sync_id
            << ", url " << entry->virtual_url().possibly_invalid_spec()
            << " and title " << entry->title();
      }
      TabNavigation tab_nav;
      tab_nav.SetFromNavigationEntry(*entry);
      sync_pb::TabNavigation* nav_s = tab_s->add_navigation();
      PopulateSessionSpecificsNavigation(&tab_nav, nav_s);
    }
  }
  tab_s->set_current_navigation_index(current_index);

  tab_node.SetSessionSpecifics(session_s);

  SessionTab* session_tab =
      synced_session_tracker_.GetSessionTab(GetCurrentMachineTag(),
                                             tab_s->tab_id(),
                                             false);
   PopulateSessionTabFromSpecifics(*tab_s,
                                  base::Time::Now().ToInternalValue(),
                                   session_tab);
   return true;
 }
