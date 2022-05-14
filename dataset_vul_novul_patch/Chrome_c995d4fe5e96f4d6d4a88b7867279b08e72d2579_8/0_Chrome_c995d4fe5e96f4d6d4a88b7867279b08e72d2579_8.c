bool DataReductionProxySettings::IsDataSaverEnabledByUser() const {
//// static
bool DataReductionProxySettings::IsDataSaverEnabledByUser(PrefService* prefs) {
   if (params::ShouldForceEnableDataReductionProxy())
     return true;
 
  return prefs && prefs->GetBoolean(prefs::kDataSaverEnabled);
 }
