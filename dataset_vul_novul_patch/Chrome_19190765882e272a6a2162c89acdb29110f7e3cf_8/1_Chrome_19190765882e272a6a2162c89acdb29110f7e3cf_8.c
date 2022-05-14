 bool SessionModelAssociator::AssociateForeignSpecifics(
     const sync_pb::SessionSpecifics& specifics,
    const base::Time& modification_time) {
   DCHECK(CalledOnValidThread());
   std::string foreign_session_tag = specifics.session_tag();
   if (foreign_session_tag == GetCurrentMachineTag() && !setup_for_test_)
    return false;

  if (specifics.has_header()) {

    SyncedSession* foreign_session =
        synced_session_tracker_.GetSession(foreign_session_tag);

    const sync_pb::SessionHeader& header = specifics.header();
    PopulateSessionHeaderFromSpecifics(header, foreign_session);
    foreign_session->windows.reserve(header.window_size());
    VLOG(1) << "Associating " << foreign_session_tag << " with " <<
        header.window_size() << " windows.";
    size_t i;
    for (i = 0; i < static_cast<size_t>(header.window_size()); ++i) {
      if (i >= foreign_session->windows.size()) {
        foreign_session->windows.push_back(new SessionWindow());
      }
      const sync_pb::SessionWindow& window_s = header.window(i);
      PopulateSessionWindowFromSpecifics(foreign_session_tag,
                                         window_s,
                                         modification_time,
                                         foreign_session->windows[i],
                                         &synced_session_tracker_);
    }
    for (; i < foreign_session->windows.size(); ++i) {
      delete foreign_session->windows[i];
    }
    foreign_session->windows.resize(header.window_size());
  } else if (specifics.has_tab()) {
    const sync_pb::SessionTab& tab_s = specifics.tab();
    SessionID::id_type tab_id = tab_s.tab_id();
    SessionTab* tab =
        synced_session_tracker_.GetSessionTab(foreign_session_tag,
                                              tab_id,
                                              false);
    PopulateSessionTabFromSpecifics(tab_s, modification_time, tab);
  } else {
    NOTREACHED();
    return false;
  }

  return true;
}
