DataReductionProxySettings::~DataReductionProxySettings() {
  spdy_proxy_auth_enabled_.Destroy();
}
