void AutofillManager::OnQueryFormFieldAutofillImpl(
    int query_id,
    const FormData& form,
    const FormFieldData& field,
    const gfx::RectF& transformed_box,
    bool autoselect_first_suggestion) {
  external_delegate_->OnQuery(query_id, form, field, transformed_box);

  std::vector<Suggestion> suggestions;
  SuggestionsContext context;
  GetAvailableSuggestions(form, field, &suggestions, &context);

  if (context.is_autofill_available) {
    switch (context.suppress_reason) {
      case SuppressReason::kNotSuppressed:
        break;

      case SuppressReason::kCreditCardsAblation:
        enable_ablation_logging_ = true;
        autocomplete_history_manager_->CancelPendingQuery();
        external_delegate_->OnSuggestionsReturned(query_id, suggestions,
                                                  autoselect_first_suggestion);
        return;

      case SuppressReason::kAutocompleteOff:
        return;
    }

    if (!suggestions.empty()) {
      if (context.is_filling_credit_card) {
        AutofillMetrics::LogIsQueriedCreditCardFormSecure(
            context.is_context_secure);
      }

      if (!has_logged_address_suggestions_count_ &&
          !context.section_has_autofilled_field) {
         AutofillMetrics::LogAddressSuggestionsCount(suggestions.size());
         has_logged_address_suggestions_count_ = true;
       }
    }
  }

  if (suggestions.empty() && !ShouldShowCreditCardSigninPromo(form, field) &&
      field.should_autocomplete &&
      !(context.focused_field &&
        (IsCreditCardExpirationType(
             context.focused_field->Type().GetStorableType()) ||
         context.focused_field->Type().html_type() == HTML_TYPE_UNRECOGNIZED ||
         context.focused_field->Type().GetStorableType() ==
             CREDIT_CARD_NUMBER ||
         context.focused_field->Type().GetStorableType() ==
             CREDIT_CARD_VERIFICATION_CODE))) {
    autocomplete_history_manager_->OnGetAutocompleteSuggestions(
        query_id, field.name, field.value, field.form_control_type);
    return;
  }

  autocomplete_history_manager_->CancelPendingQuery();
  external_delegate_->OnSuggestionsReturned(query_id, suggestions,
                                            autoselect_first_suggestion,
                                            context.is_all_server_suggestions);
}
