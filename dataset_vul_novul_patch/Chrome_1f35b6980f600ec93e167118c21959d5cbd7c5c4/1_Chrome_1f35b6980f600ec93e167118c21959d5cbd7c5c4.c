void CredentialManagerImpl::OnProvisionalSaveComplete() {
  DCHECK(form_manager_);
  DCHECK(client_->IsSavingAndFillingEnabledForCurrentPage());
  const autofill::PasswordForm& form = form_manager_->pending_credentials();

  if (form_manager_->IsPendingCredentialsPublicSuffixMatch()) {
    form_manager_->Save();
    return;
  }

  if (!form.federation_origin.unique()) {
    for (auto* match : form_manager_->form_fetcher()->GetFederatedMatches()) {
      if (match->username_value == form.username_value &&
          match->federation_origin.IsSameOriginWith(form.federation_origin)) {
        form_manager_->Update(*match);
        return;
      }
    }
  } else if (!form_manager_->IsNewLogin()) {
    form_manager_->Update(*form_manager_->preferred_match());
     return;
   }
 
  client_->PromptUserToSaveOrUpdatePassword(std::move(form_manager_), false);
}
