void PasswordAutofillManager::OnShowPasswordSuggestions(
    int key,
    base::i18n::TextDirection text_direction,
    const base::string16& typed_username,
    int options,
    const gfx::RectF& bounds) {
  std::vector<autofill::Suggestion> suggestions;
  LoginToPasswordInfoMap::const_iterator fill_data_it =
      login_to_password_info_.find(key);
  if (fill_data_it == login_to_password_info_.end()) {
    NOTREACHED();
    return;
  }
  GetSuggestions(fill_data_it->second, typed_username, &suggestions,
                 (options & autofill::SHOW_ALL) != 0,
                 (options & autofill::IS_PASSWORD_FIELD) != 0);

  form_data_key_ = key;

  if (suggestions.empty()) {
    autofill_client_->HideAutofillPopup();
    return;
  }

  if (options & autofill::IS_PASSWORD_FIELD) {
    autofill::Suggestion password_field_suggestions(l10n_util::GetStringUTF16(
        IDS_AUTOFILL_PASSWORD_FIELD_SUGGESTIONS_TITLE));
    password_field_suggestions.frontend_id = autofill::POPUP_ITEM_ID_TITLE;
    suggestions.insert(suggestions.begin(), password_field_suggestions);
  }

  GURL origin = (fill_data_it->second).origin;
  bool is_context_secure = autofill_client_->IsContextSecure() &&
                           (!origin.is_valid() || !origin.SchemeIs("http"));
  if (!is_context_secure && security_state::IsHttpWarningInFormEnabled()) {
    std::string icon_str;

    if (origin.is_valid() && origin.SchemeIs("http")) {
      icon_str = "httpWarning";
    } else {
      icon_str = "httpsInvalid";
    }

    autofill::Suggestion http_warning_suggestion(
        l10n_util::GetStringUTF8(IDS_AUTOFILL_LOGIN_HTTP_WARNING_MESSAGE),
        l10n_util::GetStringUTF8(IDS_AUTOFILL_HTTP_WARNING_LEARN_MORE),
        icon_str, autofill::POPUP_ITEM_ID_HTTP_NOT_SECURE_WARNING_MESSAGE);
#if !defined(OS_ANDROID)
      suggestions.insert(suggestions.begin(), autofill::Suggestion());
      suggestions.front().frontend_id = autofill::POPUP_ITEM_ID_SEPARATOR;
#endif
      suggestions.insert(suggestions.begin(), http_warning_suggestion);

      if (!did_show_form_not_secure_warning_) {
        did_show_form_not_secure_warning_ = true;
        metrics_util::LogShowedFormNotSecureWarningOnCurrentNavigation();
      }
  }

   if (ShouldShowManualFallbackForPreLollipop(
           autofill_client_->GetSyncService())) {
     if (base::FeatureList::IsEnabled(
            password_manager::features::kEnableManualFallbacksFilling) &&
         (options & autofill::IS_PASSWORD_FIELD) && password_client_ &&
         password_client_->IsFillingFallbackEnabledForCurrentPage()) {
       AddSimpleSuggestionWithSeparatorOnTop(
          IDS_AUTOFILL_SHOW_ALL_SAVED_FALLBACK,
          autofill::POPUP_ITEM_ID_ALL_SAVED_PASSWORDS_ENTRY, &suggestions);

      show_all_saved_passwords_shown_context_ =
          metrics_util::SHOW_ALL_SAVED_PASSWORDS_CONTEXT_PASSWORD;
      metrics_util::LogContextOfShowAllSavedPasswordsShown(
          show_all_saved_passwords_shown_context_);
    }
    if (base::FeatureList::IsEnabled(
            password_manager::features::kEnableManualFallbacksGeneration) &&
        password_manager_util::GetPasswordSyncState(
            autofill_client_->GetSyncService()) == SYNCING_NORMAL_ENCRYPTION) {
      AddSimpleSuggestionWithSeparatorOnTop(
          IDS_AUTOFILL_GENERATE_PASSWORD_FALLBACK,
          autofill::POPUP_ITEM_ID_GENERATE_PASSWORD_ENTRY, &suggestions);
    }
  }

  autofill_client_->ShowAutofillPopup(bounds,
                                      text_direction,
                                      suggestions,
                                      weak_ptr_factory_.GetWeakPtr());
}
