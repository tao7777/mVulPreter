 void UpdateContentLengthPrefsForDataReductionProxy(
    int received_content_length,
    int original_content_length,
    bool with_data_reduction_proxy_enabled,
    DataReductionRequestType data_reduction_type,
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
 
  DailyContentLengthUpdate https(
      prefs::kDailyContentLengthHttpsWithDataReductionProxyEnabled, prefs);
  https.UpdateForDataChange(days_since_last_update);

  DailyContentLengthUpdate short_bypass(
      prefs::kDailyContentLengthShortBypassWithDataReductionProxyEnabled,
      prefs);
  short_bypass.UpdateForDataChange(days_since_last_update);

  DailyContentLengthUpdate long_bypass(
      prefs::kDailyContentLengthLongBypassWithDataReductionProxyEnabled, prefs);
  long_bypass.UpdateForDataChange(days_since_last_update);

  DailyContentLengthUpdate unknown(
      prefs::kDailyContentLengthUnknownWithDataReductionProxyEnabled, prefs);
  unknown.UpdateForDataChange(days_since_last_update);

   total.Add(original_content_length, received_content_length);
   if (with_data_reduction_proxy_enabled) {
     proxy_enabled.Add(original_content_length, received_content_length);
    // Ignore data source cases, if exist, when
    // "with_data_reduction_proxy_enabled == false"
    switch (data_reduction_type) {
      case VIA_DATA_REDUCTION_PROXY:
        via_proxy.Add(original_content_length, received_content_length);
        break;
      case OFF_THE_RECORD:
        // We don't measure off-the-record data.
        break;
      case HTTPS:
        https.Add(received_content_length);
        break;
      case SHORT_BYPASS:
        short_bypass.Add(received_content_length);
        break;
      case LONG_BYPASS:
        long_bypass.Add(received_content_length);
        break;
      case UNKNOWN_TYPE:
        unknown.Add(received_content_length);
        break;
     }
   }
 
  if (days_since_last_update) {
    prefs->SetInt64(prefs::kDailyHttpContentLengthLastUpdateDate,
                    midnight.ToInternalValue());

     if (days_since_last_update == 1) {
      // Therefore (kNumDaysInHistory - 2) below.
       RecordDailyContentLengthHistograms(
           total.GetOriginalListPrefValue(kNumDaysInHistory - 2),
           total.GetReceivedListPrefValue(kNumDaysInHistory - 2),
           proxy_enabled.GetOriginalListPrefValue(kNumDaysInHistory - 2),
           proxy_enabled.GetReceivedListPrefValue(kNumDaysInHistory - 2),
           via_proxy.GetOriginalListPrefValue(kNumDaysInHistory - 2),
          via_proxy.GetReceivedListPrefValue(kNumDaysInHistory - 2),
          https.GetListPrefValue(kNumDaysInHistory - 2),
          short_bypass.GetListPrefValue(kNumDaysInHistory - 2),
          long_bypass.GetListPrefValue(kNumDaysInHistory - 2),
          unknown.GetListPrefValue(kNumDaysInHistory - 2));
     }
   }
 }
