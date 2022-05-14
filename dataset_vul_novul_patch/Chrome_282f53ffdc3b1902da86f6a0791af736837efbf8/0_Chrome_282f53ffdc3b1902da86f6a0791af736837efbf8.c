void DiceResponseHandler::OnTokenExchangeSuccess(
    DiceTokenFetcher* token_fetcher,
    const std::string& refresh_token,
    bool is_under_advanced_protection) {
  const std::string& email = token_fetcher->email();
  const std::string& gaia_id = token_fetcher->gaia_id();
  if (!CanGetTokenForAccount(gaia_id, email))
    return;
  VLOG(1) << "[Dice] OAuth success for email " << email;
  bool should_enable_sync = token_fetcher->should_enable_sync();
  std::string account_id =
       account_tracker_service_->SeedAccountInfo(gaia_id, email);
   account_tracker_service_->SetIsAdvancedProtectionAccount(
       account_id, is_under_advanced_protection);
  token_service_->UpdateCredentials(
      account_id, refresh_token,
      signin_metrics::SourceForRefreshTokenOperation::
          kDiceResponseHandler_Signin);
   about_signin_internals_->OnRefreshTokenReceived(
       base::StringPrintf("Successful (%s)", account_id.c_str()));
   if (should_enable_sync)
    token_fetcher->delegate()->EnableSync(account_id);

  DeleteTokenFetcher(token_fetcher);
}
