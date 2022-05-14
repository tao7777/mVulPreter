void DataReductionProxySettings::MaybeActivateDataReductionProxy(
    bool at_startup) {
  DCHECK(thread_checker_.CalledOnValidThread());
  PrefService* prefs = GetOriginalProfilePrefs();
   if (!prefs)
     return;
 
  if (spdy_proxy_auth_enabled_.GetValue() && at_startup) {
     int64_t last_enabled_time =
         prefs->GetInt64(prefs::kDataReductionProxyLastEnabledTime);
    if (last_enabled_time != 0) {
      RecordDaysSinceEnabledMetric(
          (clock_->Now() - base::Time::FromInternalValue(last_enabled_time))
              .InDays());
    }

    int64_t last_savings_cleared_time = prefs->GetInt64(
        prefs::kDataReductionProxySavingsClearedNegativeSystemClock);
    if (last_savings_cleared_time != 0) {
      int32_t days_since_savings_cleared =
          (clock_->Now() -
           base::Time::FromInternalValue(last_savings_cleared_time))
              .InDays();

      if (days_since_savings_cleared == 0)
        days_since_savings_cleared = 1;
      UMA_HISTOGRAM_CUSTOM_COUNTS(
          "DataReductionProxy.DaysSinceSavingsCleared.NegativeSystemClock",
          days_since_savings_cleared, 1, 365, 50);
     }
   }
 
  if (spdy_proxy_auth_enabled_.GetValue() &&
       !prefs->GetBoolean(prefs::kDataReductionProxyWasEnabledBefore)) {
     prefs->SetBoolean(prefs::kDataReductionProxyWasEnabledBefore, true);
     ResetDataReductionStatistics();
  }
  if (!at_startup) {
    if (IsDataReductionProxyEnabled()) {
      RecordSettingsEnabledState(DATA_REDUCTION_SETTINGS_ACTION_OFF_TO_ON);

      prefs->SetInt64(prefs::kDataReductionProxyLastEnabledTime,
                      clock_->Now().ToInternalValue());
      RecordDaysSinceEnabledMetric(0);
    } else {
      RecordSettingsEnabledState(DATA_REDUCTION_SETTINGS_ACTION_ON_TO_OFF);
    }
  }
  if (at_startup && !data_reduction_proxy_service_->Initialized())
    deferred_initialization_ = true;
  else
    UpdateIOData(at_startup);
}
