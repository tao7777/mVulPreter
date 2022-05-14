   ~SessionRestoreImpl() {
     STLDeleteElements(&windows_);
 
    CHECK(profiles_getting_restored);
    CHECK(profiles_getting_restored->find(profile_) !=
          profiles_getting_restored->end());
    profiles_getting_restored->erase(profile_);
    if (profiles_getting_restored->empty()) {
      delete profiles_getting_restored;
      profiles_getting_restored = NULL;
     }
     g_browser_process->ReleaseModule();
   }
