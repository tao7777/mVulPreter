void DataReductionProxySettings::InitPrefMembers() {
  DCHECK(thread_checker_.CalledOnValidThread());
  spdy_proxy_auth_enabled_.Init(
      prefs::kDataSaverEnabled, GetOriginalProfilePrefs(),
      base::Bind(&DataReductionProxySettings::OnProxyEnabledPrefChange,
                 base::Unretained(this)));
}
