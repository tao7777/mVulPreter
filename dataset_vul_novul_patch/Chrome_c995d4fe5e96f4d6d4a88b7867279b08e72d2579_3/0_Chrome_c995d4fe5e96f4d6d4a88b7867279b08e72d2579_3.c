PrefService* DataReductionProxySettings::GetOriginalProfilePrefs() {
PrefService* DataReductionProxySettings::GetOriginalProfilePrefs() const {
   DCHECK(thread_checker_.CalledOnValidThread());
   return prefs_;
 }
