void SaveCardBubbleControllerImpl::ShowBubbleForUpload(
    const CreditCard& card,
     std::unique_ptr<base::DictionaryValue> legal_message,
     bool should_cvc_be_requested,
     const base::Closure& save_card_callback) {
  // Don't show the bubble if it's already visible.
  if (save_card_bubble_view_)
    return;

   is_uploading_ = true;
   is_reshow_ = false;
   should_cvc_be_requested_ = should_cvc_be_requested;
  AutofillMetrics::LogSaveCardPromptMetric(
      AutofillMetrics::SAVE_CARD_PROMPT_SHOW_REQUESTED, is_uploading_,
      is_reshow_,
      pref_service_->GetInteger(
          prefs::kAutofillAcceptSaveCreditCardPromptState));

  if (!LegalMessageLine::Parse(*legal_message, &legal_message_lines_)) {
    AutofillMetrics::LogSaveCardPromptMetric(
        AutofillMetrics::SAVE_CARD_PROMPT_END_INVALID_LEGAL_MESSAGE,
        is_uploading_, is_reshow_,
        pref_service_->GetInteger(
            prefs::kAutofillAcceptSaveCreditCardPromptState));
    return;
  }

  card_ = card;
  save_card_callback_ = save_card_callback;
  ShowBubble();
}
