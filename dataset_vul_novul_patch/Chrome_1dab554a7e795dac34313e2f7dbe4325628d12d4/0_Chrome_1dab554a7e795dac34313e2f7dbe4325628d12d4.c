 bool SessionRestore::IsRestoring(const Profile* profile) {
  if (active_session_restorers == NULL)
    return false;
  for (std::set<SessionRestoreImpl*>::const_iterator it =
           active_session_restorers->begin();
       it != active_session_restorers->end(); ++it) {
    if ((*it)->profile() == profile)
      return true;
  }
  return false;
 }
