void ContextualSearchDelegate::DecodeSearchTermFromJsonResponse(
    const std::string& response,
    std::string* search_term,
    std::string* display_text,
    std::string* alternate_term,
    std::string* mid,
    std::string* prevent_preload,
    int* mention_start,
    int* mention_end,
    std::string* lang,
    std::string* thumbnail_url,
    std::string* caption) {
  bool contains_xssi_escape =
      base::StartsWith(response, kXssiEscape, base::CompareCase::SENSITIVE);
  const std::string& proper_json =
      contains_xssi_escape ? response.substr(sizeof(kXssiEscape) - 1)
                           : response;
  JSONStringValueDeserializer deserializer(proper_json);
  std::unique_ptr<base::Value> root =
      deserializer.Deserialize(nullptr, nullptr);
  const std::unique_ptr<base::DictionaryValue> dict =
      base::DictionaryValue::From(std::move(root));
  if (!dict)
    return;

  dict->GetString(kContextualSearchPreventPreload, prevent_preload);
  dict->GetString(kContextualSearchResponseSearchTermParam, search_term);
  dict->GetString(kContextualSearchResponseLanguageParam, lang);

  if (!dict->GetString(kContextualSearchResponseDisplayTextParam,
                       display_text)) {
    *display_text = *search_term;
  }
  dict->GetString(kContextualSearchResponseMidParam, mid);

  if (!field_trial_->IsDecodeMentionsDisabled()) {
    base::ListValue* mentions_list = nullptr;
    dict->GetList(kContextualSearchMentions, &mentions_list);
    if (mentions_list && mentions_list->GetSize() >= 2)
      ExtractMentionsStartEnd(*mentions_list, mention_start, mention_end);
  }

  std::string selected_text;
  dict->GetString(kContextualSearchResponseSelectedTextParam, &selected_text);
  if (selected_text != *search_term) {
    *alternate_term = selected_text;
  } else {
    std::string resolved_term;
    dict->GetString(kContextualSearchResponseResolvedTermParam, &resolved_term);
    if (resolved_term != *search_term) {
      *alternate_term = resolved_term;
     }
   }
 
  if (field_trial_->IsNowOnTapBarIntegrationEnabled()) {
     dict->GetString(kContextualSearchCaption, caption);
     dict->GetString(kContextualSearchThumbnail, thumbnail_url);
   }
}
