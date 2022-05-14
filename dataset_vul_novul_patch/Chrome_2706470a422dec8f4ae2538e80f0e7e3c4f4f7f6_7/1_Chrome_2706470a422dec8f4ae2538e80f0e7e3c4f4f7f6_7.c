bool PaymentRequest::SatisfiesSkipUIConstraints() const {
  return base::FeatureList::IsEnabled(features::kWebPaymentsSingleAppUiSkip) &&
         base::FeatureList::IsEnabled(::features::kServiceWorkerPaymentApps) &&
         is_show_user_gesture_ && state()->is_get_all_instruments_finished() &&
         state()->available_instruments().size() == 1 &&
         spec()->stringified_method_data().size() == 1 &&
         !spec()->request_shipping() && !spec()->request_payer_name() &&
         !spec()->request_payer_phone() &&
         !spec()->request_payer_email()
         && spec()->url_payment_method_identifiers().size() == 1;
}
