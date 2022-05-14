 bool DataReductionProxySettings::IsDataReductionProxyManaged() {
  const PrefService::Preference* pref =
      GetOriginalProfilePrefs()->FindPreference(prefs::kDataSaverEnabled);
  return pref && pref->IsManaged();
 }
