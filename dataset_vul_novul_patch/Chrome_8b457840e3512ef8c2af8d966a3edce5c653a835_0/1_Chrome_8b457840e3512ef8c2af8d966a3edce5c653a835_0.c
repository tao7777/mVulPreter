 void SaveCardBubbleControllerImpl::ShowBubbleForLocalSave(
     const CreditCard& card,
     const base::Closure& save_card_callback) {
   is_uploading_ = false;
   is_reshow_ = false;
   should_cvc_be_requested_ = false;
  legal_message_lines_.clear();

  AutofillMetrics::LogSaveCardPromptMetric(
      AutofillMetrics::SAVE_CARD_PROMPT_SHOW_REQUESTED, is_uploading_,
      is_reshow_,
      pref_service_->GetInteger(
          prefs::kAutofillAcceptSaveCreditCardPromptState));

  card_ = card;
  save_card_callback_ = save_card_callback;
  ShowBubble();
}
