   void UpdateForDataChange(int days_since_last_update) {
    MaintainContentLengthPrefsWindow(original_update_.Get(), kNumDaysInHistory);
    MaintainContentLengthPrefsWindow(received_update_.Get(), kNumDaysInHistory);
    if (days_since_last_update) {
      MaintainContentLengthPrefsForDateChange(
          original_update_.Get(), received_update_.Get(),
          days_since_last_update);
    }
   }
