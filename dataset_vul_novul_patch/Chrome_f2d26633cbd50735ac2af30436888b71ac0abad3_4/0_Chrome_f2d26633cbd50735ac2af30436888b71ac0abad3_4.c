void AutofillExternalDelegate::OnSuggestionsReturned(
    int query_id,
    const std::vector<Suggestion>& input_suggestions,
    bool autoselect_first_suggestion,
    bool is_all_server_suggestions) {
  if (query_id != query_id_)
    return;

  std::vector<Suggestion> suggestions(input_suggestions);

   PossiblyRemoveAutofillWarnings(&suggestions);
 
   if (should_show_scan_credit_card_) {
     Suggestion scan_credit_card(
         l10n_util::GetStringUTF16(IDS_AUTOFILL_SCAN_CREDIT_CARD));
    scan_credit_card.frontend_id = POPUP_ITEM_ID_SCAN_CREDIT_CARD;
    scan_credit_card.icon = base::ASCIIToUTF16("scanCreditCardIcon");
    suggestions.push_back(scan_credit_card);
  }

  has_autofill_suggestions_ = false;
  for (size_t i = 0; i < suggestions.size(); ++i) {
    if (suggestions[i].frontend_id > 0) {
      has_autofill_suggestions_ = true;
      break;
    }
  }

  if (should_show_cards_from_account_option_) {
    suggestions.emplace_back(
        l10n_util::GetStringUTF16(IDS_AUTOFILL_SHOW_ACCOUNT_CARDS));
    suggestions.back().frontend_id = POPUP_ITEM_ID_SHOW_ACCOUNT_CARDS;
    suggestions.back().icon = base::ASCIIToUTF16("google");
  }

  if (has_autofill_suggestions_)
    ApplyAutofillOptions(&suggestions, is_all_server_suggestions);

   if (suggestions.empty() && should_show_cc_signin_promo_) {
     Suggestion signin_promo_suggestion(
         l10n_util::GetStringUTF16(IDS_AUTOFILL_CREDIT_CARD_SIGNIN_PROMO));
     signin_promo_suggestion.frontend_id =
        POPUP_ITEM_ID_CREDIT_CARD_SIGNIN_PROMO;
    suggestions.push_back(signin_promo_suggestion);
    signin_metrics::RecordSigninImpressionUserActionForAccessPoint(
         signin_metrics::AccessPoint::ACCESS_POINT_AUTOFILL_DROPDOWN);
   }
 
  InsertDataListValues(&suggestions);

  if (suggestions.empty()) {
    manager_->client()->HideAutofillPopup();
    return;
  }

  if (query_field_.is_focusable) {
    manager_->client()->ShowAutofillPopup(
        element_bounds_, query_field_.text_direction, suggestions,
        autoselect_first_suggestion, GetWeakPtr());
  }
}
