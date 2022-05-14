   ~SessionRestoreImpl() {
     STLDeleteElements(&windows_);
 
    active_session_restorers->erase(this);
    if (active_session_restorers->empty()) {
      delete active_session_restorers;
      active_session_restorers = NULL;
     }

     g_browser_process->ReleaseModule();
   }
