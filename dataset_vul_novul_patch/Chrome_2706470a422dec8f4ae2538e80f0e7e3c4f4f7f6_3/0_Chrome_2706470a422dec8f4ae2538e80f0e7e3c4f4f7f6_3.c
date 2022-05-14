void PaymentRequest::Init(mojom::PaymentRequestClientPtr client,
                          std::vector<mojom::PaymentMethodDataPtr> method_data,
                           mojom::PaymentDetailsPtr details,
                           mojom::PaymentOptionsPtr options) {
   DCHECK_CURRENTLY_ON(content::BrowserThread::UI);

  if (is_initialized_) {
    log_.Error("Attempted initialization twice");
    OnConnectionTerminated();
    return;
  }

  is_initialized_ = true;
   client_ = std::move(client);
 
   const GURL last_committed_url = delegate_->GetLastCommittedURL();
   if (!OriginSecurityChecker::IsOriginSecure(last_committed_url)) {
    log_.Error("Not in a secure origin");
     OnConnectionTerminated();
     return;
   }

  bool allowed_origin =
       OriginSecurityChecker::IsSchemeCryptographic(last_committed_url) ||
       OriginSecurityChecker::IsOriginLocalhostOrFile(last_committed_url);
   if (!allowed_origin) {
    log_.Error(
        "Only localhost, file://, and cryptographic scheme origins allowed");
   }
 
   bool invalid_ssl =
       OriginSecurityChecker::IsSchemeCryptographic(last_committed_url) &&
       !delegate_->IsSslCertificateValid();
  if (invalid_ssl) {
    log_.Error("SSL certificate is not valid.");
  }
 
   if (!allowed_origin || invalid_ssl) {
    // Intentionally don't set |spec_| and |state_|, so the UI is never shown.
    log_.Error(
        "No UI will be shown. CanMakePayment will always return false. "
        "Show will be rejected with NotSupportedError.");
     return;
   }
 
   std::string error;
   if (!ValidatePaymentDetails(ConvertPaymentDetails(details), &error)) {
    log_.Error(error);
     OnConnectionTerminated();
     return;
   }
 
   if (!details->total) {
    log_.Error("Missing total");
     OnConnectionTerminated();
     return;
   }

  spec_ = std::make_unique<PaymentRequestSpec>(
      std::move(options), std::move(details), std::move(method_data), this,
      delegate_->GetApplicationLocale());
  state_ = std::make_unique<PaymentRequestState>(
      web_contents_, top_level_origin_, frame_origin_, spec_.get(), this,
      delegate_->GetApplicationLocale(), delegate_->GetPersonalDataManager(),
      delegate_.get(), &journey_logger_);

  journey_logger_.SetRequestedInformation(
      spec_->request_shipping(), spec_->request_payer_email(),
      spec_->request_payer_phone(), spec_->request_payer_name());

  GURL google_pay_url(kGooglePayMethodName);
  GURL android_pay_url(kAndroidPayMethodName);
  auto non_google_it =
      std::find_if(spec_->url_payment_method_identifiers().begin(),
                   spec_->url_payment_method_identifiers().end(),
                   [google_pay_url, android_pay_url](const GURL& url) {
                     return url != google_pay_url && url != android_pay_url;
                   });
  journey_logger_.SetRequestedPaymentMethodTypes(
      /*requested_basic_card=*/!spec_->supported_card_networks().empty(),
      /*requested_method_google=*/
      base::ContainsValue(spec_->url_payment_method_identifiers(),
                          google_pay_url) ||
          base::ContainsValue(spec_->url_payment_method_identifiers(),
                              android_pay_url),
      /*requested_method_other=*/non_google_it !=
          spec_->url_payment_method_identifiers().end());
 }
