void MaintainContentLengthPrefsForDateChange(
//// DailyContentLengthUpdate maintains a data saving pref. The pref is a list
//// of |kNumDaysInHistory| elements of daily total content lengths for the past
//// |kNumDaysInHistory| days.
class DailyContentLengthUpdate {
 public:
  DailyContentLengthUpdate(
      const char* pref,
      PrefService* pref_service)
      : update_(pref_service, pref) {
   }

  void UpdateForDataChange(int days_since_last_update) {
    // New empty lists may have been created. Maintain the invariant that
    // there should be exactly |kNumDaysInHistory| days in the histories.
    MaintainContentLengthPrefsWindow(update_.Get(), kNumDaysInHistory);
    if (days_since_last_update) {
      MaintainContentLengthPrefForDateChange(days_since_last_update);
    }
   }
 
