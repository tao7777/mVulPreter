bool DataReductionProxySettings::IsDataSaverEnabledByUser() const {
//// static
   if (params::ShouldForceEnableDataReductionProxy())
     return true;
 
  if (spdy_proxy_auth_enabled_.GetPrefName().empty())
    return false;
  return spdy_proxy_auth_enabled_.GetValue();
 }
