std::string ContextualSearchDelegate::BuildRequestUrl(std::string selection) {
  if (!template_url_service_ ||
      !template_url_service_->GetDefaultSearchProvider()) {
    return std::string();
  }

  std::string selected_text(net::EscapeQueryParamValue(selection, true));
  TemplateURL* template_url = template_url_service_->GetDefaultSearchProvider();

   TemplateURLRef::SearchTermsArgs search_terms_args =
       TemplateURLRef::SearchTermsArgs(base::string16());
 
  int contextual_cards_version =
      field_trial_->IsContextualCardsBarIntegrationEnabled()
      ? kContextualCardsVersion : 0;
 
   TemplateURLRef::SearchTermsArgs::ContextualSearchParams params(
       kContextualSearchRequestVersion, selected_text, std::string(),
      contextual_cards_version);
 
   search_terms_args.contextual_search_params = params;
 
  std::string request(
      template_url->contextual_search_url_ref().ReplaceSearchTerms(
          search_terms_args,
          template_url_service_->search_terms_data(),
          NULL));

  std::string replacement_url = field_trial_->GetResolverURLPrefix();

  if (!replacement_url.empty()) {
    size_t pos = request.find(kContextualSearchServerEndpoint);
    if (pos != std::string::npos) {
      request.replace(0, pos + strlen(kContextualSearchServerEndpoint),
                      replacement_url);
    }
  }
  return request;
}
