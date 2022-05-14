 void SaveCardBubbleControllerImpl::ReshowBubble() {
   is_reshow_ = true;
   AutofillMetrics::LogSaveCardPromptMetric(
       AutofillMetrics::SAVE_CARD_PROMPT_SHOW_REQUESTED, is_uploading_,
      is_reshow_,
      pref_service_->GetInteger(
          prefs::kAutofillAcceptSaveCreditCardPromptState));

  ShowBubble();
}
