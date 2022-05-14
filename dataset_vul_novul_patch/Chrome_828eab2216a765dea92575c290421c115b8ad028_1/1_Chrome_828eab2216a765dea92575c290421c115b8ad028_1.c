   DailyDataSavingUpdate(
      const char* pref_original, const char* pref_received,
       PrefService* pref_service)
      : pref_original_(pref_original),
        pref_received_(pref_received),
        original_update_(pref_service, pref_original_),
        received_update_(pref_service, pref_received_) {
   }
