void ZeroSuggestProvider::Start(const AutocompleteInput& input,
                                bool minimal_changes) {
  TRACE_EVENT0("omnibox", "ZeroSuggestProvider::Start");
  matches_.clear();
  if (!input.from_omnibox_focus() || client()->IsOffTheRecord() ||
      input.type() == metrics::OmniboxInputType::INVALID)
    return;

  Stop(true, false);
  set_field_trial_triggered(false);
  set_field_trial_triggered_in_session(false);
  results_from_cache_ = false;
  permanent_text_ = input.text();
  current_query_ = input.current_url().spec();
  current_title_ = input.current_title();
   current_page_classification_ = input.current_page_classification();
   current_url_match_ = MatchForCurrentURL();
 
  GURL suggest_url(GetContextualSuggestionsUrl());
   if (!suggest_url.is_valid())
     return;
 
  const TemplateURLService* template_url_service =
      client()->GetTemplateURLService();
  const TemplateURL* default_provider =
      template_url_service->GetDefaultSearchProvider();
  const bool can_send_current_url =
      CanSendURL(input.current_url(), suggest_url, default_provider,
                 current_page_classification_,
                 template_url_service->search_terms_data(), client());
  GURL arbitrary_insecure_url(kArbitraryInsecureUrlString);
  ZeroSuggestEligibility eligibility = ZeroSuggestEligibility::ELIGIBLE;
  if (!can_send_current_url) {
    const bool can_send_ordinary_url =
        CanSendURL(arbitrary_insecure_url, suggest_url, default_provider,
                   current_page_classification_,
                   template_url_service->search_terms_data(), client());
    eligibility = can_send_ordinary_url
                      ? ZeroSuggestEligibility::URL_INELIGIBLE
                      : ZeroSuggestEligibility::GENERALLY_INELIGIBLE;
  }
  UMA_HISTOGRAM_ENUMERATION(
      "Omnibox.ZeroSuggest.Eligible.OnFocus", static_cast<int>(eligibility),
      static_cast<int>(ZeroSuggestEligibility::ELIGIBLE_MAX_VALUE));
  if (can_send_current_url &&
       !OmniboxFieldTrial::InZeroSuggestPersonalizedFieldTrial() &&
       !OmniboxFieldTrial::InZeroSuggestMostVisitedFieldTrial()) {
    if (UseExperimentalSuggestService(*template_url_service)) {
      suggest_url = GURL(
          OmniboxFieldTrial::ZeroSuggestRedirectToChromeServerAddress() +
           "/url=" + net::EscapePath(current_query_) +
          OmniboxFieldTrial::ZeroSuggestRedirectToChromeAdditionalFields());
     } else {
       base::string16 prefix;
       TemplateURLRef::SearchTermsArgs search_term_args(prefix);
      search_term_args.current_page_url = current_query_;
      suggest_url =
          GURL(default_provider->suggestions_url_ref().ReplaceSearchTerms(
              search_term_args, template_url_service->search_terms_data()));
    }
  } else if (!ShouldShowNonContextualZeroSuggest(input.current_url())) {
    return;
  }

  done_ = false;
  MaybeUseCachedSuggestions();
  Run(suggest_url);
}
