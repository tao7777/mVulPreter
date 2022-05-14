   DailyDataSavingUpdate(
      const char* pref_original,
      const char* pref_received,
       PrefService* pref_service)
      : original_(pref_original, pref_service),
        received_(pref_received, pref_service) {
   }
