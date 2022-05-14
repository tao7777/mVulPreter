 void DataReductionProxySettings::InitDataReductionProxySettings(
     PrefService* prefs,
    DataReductionProxyIOData* io_data,
    std::unique_ptr<DataReductionProxyService> data_reduction_proxy_service) {
  DCHECK(thread_checker_.CalledOnValidThread());
  DCHECK(prefs);
  DCHECK(io_data);
  DCHECK(io_data->config());
  DCHECK(data_reduction_proxy_service);
  prefs_ = prefs;
   config_ = io_data->config();
   data_reduction_proxy_service_ = std::move(data_reduction_proxy_service);
   data_reduction_proxy_service_->AddObserver(this);
   RecordDataReductionInit();
 
  registrar_.Init(prefs_);
  registrar_.Add(
      prefs::kDataSaverEnabled,
      base::BindRepeating(&DataReductionProxySettings::OnProxyEnabledPrefChange,
                          base::Unretained(this)));

 #if defined(OS_ANDROID)
  if (IsDataSaverEnabledByUser(prefs_)) {
     data_reduction_proxy_service_->compression_stats()
         ->SetDataUsageReportingEnabled(true);
   }
#endif  // defined(OS_ANDROID)

  for (auto& observer : observers_)
    observer.OnSettingsInitialized();
}
