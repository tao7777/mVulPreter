 void UpdateContentLengthPrefsForDataReductionProxy(
    int received_content_length, int original_content_length,
    bool with_data_reduction_proxy_enabled, bool via_data_reduction_proxy,
     base::Time now, PrefService* prefs) {
  base::TimeDelta time_since_unix_epoch = now - base::Time::UnixEpoch();
  const int kMinDaysSinceUnixEpoch = 365 * 2;  // 2 years.
  const int kMaxDaysSinceUnixEpoch = 365 * 1000;  // 1000 years.
  if (time_since_unix_epoch.InDays() < kMinDaysSinceUnixEpoch ||
      time_since_unix_epoch.InDays() > kMaxDaysSinceUnixEpoch) {
    return;
  }

  int64 then_internal = prefs->GetInt64(
      prefs::kDailyHttpContentLengthLastUpdateDate);
  base::Time then_midnight =
      base::Time::FromInternalValue(then_internal).LocalMidnight();
  base::Time midnight = now.LocalMidnight();
  int days_since_last_update = (midnight - then_midnight).InDays();

  DailyDataSavingUpdate total(
      prefs::kDailyHttpOriginalContentLength,
      prefs::kDailyHttpReceivedContentLength,
      prefs);
  total.UpdateForDataChange(days_since_last_update);

  DailyDataSavingUpdate proxy_enabled(
      prefs::kDailyOriginalContentLengthWithDataReductionProxyEnabled,
      prefs::kDailyContentLengthWithDataReductionProxyEnabled,
      prefs);
  proxy_enabled.UpdateForDataChange(days_since_last_update);

  DailyDataSavingUpdate via_proxy(
      prefs::kDailyOriginalContentLengthViaDataReductionProxy,
      prefs::kDailyContentLengthViaDataReductionProxy,
       prefs);
   via_proxy.UpdateForDataChange(days_since_last_update);
 
   total.Add(original_content_length, received_content_length);
   if (with_data_reduction_proxy_enabled) {
     proxy_enabled.Add(original_content_length, received_content_length);
    if (via_data_reduction_proxy) {
      via_proxy.Add(original_content_length, received_content_length);
     }
   }
 
  if (days_since_last_update) {
    prefs->SetInt64(prefs::kDailyHttpContentLengthLastUpdateDate,
                    midnight.ToInternalValue());

     if (days_since_last_update == 1) {
       RecordDailyContentLengthHistograms(
           total.GetOriginalListPrefValue(kNumDaysInHistory - 2),
           total.GetReceivedListPrefValue(kNumDaysInHistory - 2),
           proxy_enabled.GetOriginalListPrefValue(kNumDaysInHistory - 2),
           proxy_enabled.GetReceivedListPrefValue(kNumDaysInHistory - 2),
           via_proxy.GetOriginalListPrefValue(kNumDaysInHistory - 2),
          via_proxy.GetReceivedListPrefValue(kNumDaysInHistory - 2));
     }
   }
 }
