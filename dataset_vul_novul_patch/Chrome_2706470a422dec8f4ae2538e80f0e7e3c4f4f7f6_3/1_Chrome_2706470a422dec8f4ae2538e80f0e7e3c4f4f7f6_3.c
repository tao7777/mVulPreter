void PaymentRequest::Init(mojom::PaymentRequestClientPtr client,
                          std::vector<mojom::PaymentMethodDataPtr> method_data,
                           mojom::PaymentDetailsPtr details,
                           mojom::PaymentOptionsPtr options) {
   DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
   client_ = std::move(client);
 
   const GURL last_committed_url = delegate_->GetLastCommittedURL();
   if (!OriginSecurityChecker::IsOriginSecure(last_committed_url)) {
    LOG(ERROR) << "Not in a secure origin";
     OnConnectionTerminated();
     return;
   }

  bool allowed_origin =
       OriginSecurityChecker::IsSchemeCryptographic(last_committed_url) ||
       OriginSecurityChecker::IsOriginLocalhostOrFile(last_committed_url);
   if (!allowed_origin) {
    LOG(ERROR) << "Only localhost, file://, and cryptographic scheme origins "
                  "allowed";
   }
 
   bool invalid_ssl =
       OriginSecurityChecker::IsSchemeCryptographic(last_committed_url) &&
       !delegate_->IsSslCertificateValid();
  if (invalid_ssl)
    LOG(ERROR) << "SSL certificate is not valid";
 
   if (!allowed_origin || invalid_ssl) {
     return;
   }
 
   std::string error;
   if (!ValidatePaymentDetails(ConvertPaymentDetails(details), &error)) {
    LOG(ERROR) << error;
     OnConnectionTerminated();
     return;
   }
 
   if (!details->total) {
    LOG(ERROR) << "Missing total";
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
