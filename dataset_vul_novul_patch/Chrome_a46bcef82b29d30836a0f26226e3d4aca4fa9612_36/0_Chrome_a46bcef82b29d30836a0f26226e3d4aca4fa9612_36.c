 JingleSessionManager::~JingleSessionManager() {
  // Session manager can be destroyed only after all sessions are destroyed.
  DCHECK(sessions_.empty());
   Close();
 }
