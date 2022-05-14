bool ContextualSearchFieldTrial::IsNowOnTapBarIntegrationEnabled() {
   return GetBooleanParam(
      switches::kEnableContextualSearchNowOnTapBarIntegration,
      &is_now_on_tap_bar_integration_enabled_cached_,
      &is_now_on_tap_bar_integration_enabled_);
 }
