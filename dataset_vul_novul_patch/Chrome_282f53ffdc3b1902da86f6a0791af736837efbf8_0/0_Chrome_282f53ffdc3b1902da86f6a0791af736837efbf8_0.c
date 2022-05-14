void DiceResponseHandler::ProcessDiceSignoutHeader(
    const std::vector<signin::DiceResponseParams::AccountInfo>& account_infos) {
  VLOG(1) << "Start processing Dice signout response";
  if (account_consistency_ ==
      signin::AccountConsistencyMethod::kDiceFixAuthErrors) {
    return;
  }

  std::string primary_account = signin_manager_->GetAuthenticatedAccountId();
  bool primary_account_signed_out = false;
  for (const auto& account_info : account_infos) {
    std::string signed_out_account =
        account_tracker_service_->PickAccountIdForAccount(account_info.gaia_id,
                                                          account_info.email);
    if (signed_out_account == primary_account) {
      primary_account_signed_out = true;
      RecordDiceResponseHeader(kSignoutPrimary);
      RecordGaiaSignoutMetrics(
          (account_info.session_index == 0)
              ? kChromePrimaryAccountIsFirstGaiaAccount
              : kChromePrimaryAccountIsSecondaryGaiaAccount);

      if (account_consistency_ == signin::AccountConsistencyMethod::kDice) {
         token_service_->UpdateCredentials(
             primary_account,
            MutableProfileOAuth2TokenServiceDelegate::kInvalidRefreshToken,
            signin_metrics::SourceForRefreshTokenOperation::
                kDiceResponseHandler_Signout);
       } else {
         continue;
       }
     } else {
      token_service_->RevokeCredentials(
          signed_out_account, signin_metrics::SourceForRefreshTokenOperation::
                                  kDiceResponseHandler_Signout);
     }
 
    for (auto it = token_fetchers_.begin(); it != token_fetchers_.end(); ++it) {
      std::string token_fetcher_account_id =
          account_tracker_service_->PickAccountIdForAccount(
              it->get()->gaia_id(), it->get()->email());
      if (token_fetcher_account_id == signed_out_account) {
        token_fetchers_.erase(it);
        break;
      }
    }
  }

  if (!primary_account_signed_out) {
    RecordDiceResponseHeader(kSignoutSecondary);
    RecordGaiaSignoutMetrics(primary_account.empty()
                                 ? kNoChromePrimaryAccount
                                 : kChromePrimaryAccountIsNotInGaiaAccounts);
  }
}
