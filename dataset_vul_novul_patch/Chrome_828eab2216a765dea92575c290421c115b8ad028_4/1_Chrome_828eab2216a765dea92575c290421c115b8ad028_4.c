void MaintainContentLengthPrefsForDateChange(
    base::ListValue* original_update,
    base::ListValue* received_update,
    int days_since_last_update) {
  if (days_since_last_update == -1) {
    days_since_last_update = 0;
  } else if (days_since_last_update < -1) {
    original_update->Clear();
    received_update->Clear();
    days_since_last_update = kNumDaysInHistory;
//// DailyContentLengthUpdate maintains a data saving pref. The pref is a list
//// of |kNumDaysInHistory| elements of daily total content lengths for the past
//// |kNumDaysInHistory| days.
   }
  DCHECK_GE(days_since_last_update, 0);
  for (int i = 0;
       i < days_since_last_update && i < static_cast<int>(kNumDaysInHistory);
       ++i) {
    original_update->AppendString(base::Int64ToString(0));
    received_update->AppendString(base::Int64ToString(0));
   }
 
  MaintainContentLengthPrefsWindow(original_update, kNumDaysInHistory);
  MaintainContentLengthPrefsWindow(received_update, kNumDaysInHistory);
}
