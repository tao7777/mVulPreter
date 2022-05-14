AutocompleteLog::AutocompleteLog(
    const string16& text,
    bool just_deleted_text,
    AutocompleteInput::Type input_type,
    size_t selected_index,
    SessionID::id_type tab_id,
    metrics::OmniboxEventProto::PageClassification current_page_classification,
    base::TimeDelta elapsed_time_since_user_first_modified_omnibox,
    size_t inline_autocompleted_length,
    const AutocompleteResult& result)
    : text(text),
      just_deleted_text(just_deleted_text),
      input_type(input_type),
      selected_index(selected_index),
      tab_id(tab_id),
      current_page_classification(current_page_classification),
       elapsed_time_since_user_first_modified_omnibox(
           elapsed_time_since_user_first_modified_omnibox),
       inline_autocompleted_length(inline_autocompleted_length),
      result(result) {
 }
