void AutofillManager::GetAvailableSuggestions(
    const FormData& form,
    const FormFieldData& field,
    std::vector<Suggestion>* suggestions,
    SuggestionsContext* context) {
  DCHECK(suggestions);
  DCHECK(context);

  bool is_autofill_possible = RefreshDataModels();

  bool got_autofillable_form =
      GetCachedFormAndField(form, field, &context->form_structure,
                            &context->focused_field) &&
      context->form_structure->ShouldBeParsed();

  if (got_autofillable_form) {
    if (context->focused_field->Type().group() == CREDIT_CARD) {
      context->is_filling_credit_card = true;
      driver()->DidInteractWithCreditCardForm();
      credit_card_form_event_logger_->OnDidInteractWithAutofillableForm(
          context->form_structure->form_signature());
    } else {
      address_form_event_logger_->OnDidInteractWithAutofillableForm(
          context->form_structure->form_signature());
    }
  }

  context->is_context_secure =
      !IsFormNonSecure(form) ||
      !base::FeatureList::IsEnabled(
          features::kAutofillRequireSecureCreditCardContext);


  if (!is_autofill_possible || !driver()->RendererIsAvailable() ||
      !got_autofillable_form)
    return;

  context->is_autofill_available = true;

  if (context->is_filling_credit_card) {
    *suggestions =
        GetCreditCardSuggestions(field, context->focused_field->Type(),
                                 &context->is_all_server_suggestions);

    if (base::FeatureList::IsEnabled(kAutofillCreditCardAblationExperiment) &&
        !suggestions->empty()) {
      context->suppress_reason = SuppressReason::kCreditCardsAblation;
      suggestions->clear();
      return;
    }
  } else {
    if (!base::FeatureList::IsEnabled(kAutofillAlwaysFillAddresses) &&
        IsDesktopPlatform() && !field.should_autocomplete) {
      context->suppress_reason = SuppressReason::kAutocompleteOff;
      return;
    }

    *suggestions = GetProfileSuggestions(*context->form_structure, field,
                                         *context->focused_field);
  }

  if (!suggestions->empty() && context->is_filling_credit_card &&
      !context->is_context_secure) {
    Suggestion warning_suggestion(
        l10n_util::GetStringUTF16(IDS_AUTOFILL_WARNING_INSECURE_CONNECTION));
     warning_suggestion.frontend_id =
         POPUP_ITEM_ID_INSECURE_CONTEXT_PAYMENT_DISABLED_MESSAGE;
     suggestions->assign(1, warning_suggestion);
   }
 }
