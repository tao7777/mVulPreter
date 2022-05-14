 bool DataReductionProxySettings::IsDataReductionProxyEnabled() const {
   if (base::FeatureList::IsEnabled(network::features::kNetworkService) &&
       !params::IsEnabledWithNetworkService()) {
     return false;
   }
  return IsDataSaverEnabledByUser(GetOriginalProfilePrefs());
 }
