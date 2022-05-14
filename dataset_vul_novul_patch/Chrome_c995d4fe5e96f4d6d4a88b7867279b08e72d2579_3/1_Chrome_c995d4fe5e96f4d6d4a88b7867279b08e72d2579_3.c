PrefService* DataReductionProxySettings::GetOriginalProfilePrefs() {
   DCHECK(thread_checker_.CalledOnValidThread());
   return prefs_;
 }
