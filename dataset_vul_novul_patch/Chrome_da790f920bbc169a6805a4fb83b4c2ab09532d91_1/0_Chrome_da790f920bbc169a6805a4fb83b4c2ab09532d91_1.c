bool TestDataReductionProxyConfig::ShouldAddDefaultProxyBypassRules() const {
void TestDataReductionProxyConfig::AddDefaultProxyBypassRules() {
  if (!add_default_proxy_bypass_rules_) {
    // Set bypass rules which allow proxying localhost.
    configurator_->SetBypassRules(
        net::ProxyBypassRules::GetRulesToSubtractImplicit());
  }
 }
