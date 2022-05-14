bool ContextualSearchFieldTrial::IsNowOnTapBarIntegrationEnabled() {
bool ContextualSearchFieldTrial::IsContextualCardsBarIntegrationEnabled() {
   return GetBooleanParam(
      switches::kEnableContextualSearchContextualCardsBarIntegration,
      &is_contextual_cards_bar_integration_enabled_cached_,
      &is_contextual_cards_bar_integration_enabled_);
 }
