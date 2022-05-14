 void DataReductionProxySettings::SetDataReductionProxyEnabled(bool enabled) {
   DCHECK(thread_checker_.CalledOnValidThread());
   DCHECK(data_reduction_proxy_service_->compression_stats());
  if (GetOriginalProfilePrefs()->GetBoolean(prefs::kDataSaverEnabled) !=
      enabled) {
    GetOriginalProfilePrefs()->SetBoolean(prefs::kDataSaverEnabled, enabled);
     OnProxyEnabledPrefChange();
 #if defined(OS_ANDROID)
     data_reduction_proxy_service_->compression_stats()
        ->SetDataUsageReportingEnabled(enabled);
#endif  // defined(OS_ANDROID)
  }
}
