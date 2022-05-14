void UserSelectionScreen::FillUserMojoStruct(
    const user_manager::User* user,
    bool is_owner,
    bool is_signin_to_add,
    proximity_auth::mojom::AuthType auth_type,
    const std::vector<std::string>* public_session_recommended_locales,
    ash::mojom::LoginUserInfo* user_info) {
  user_info->basic_user_info = ash::mojom::UserInfo::New();
  user_info->basic_user_info->type = user->GetType();
  user_info->basic_user_info->account_id = user->GetAccountId();
  user_info->basic_user_info->display_name =
      base::UTF16ToUTF8(user->GetDisplayName());
  user_info->basic_user_info->display_email = user->display_email();
  user_info->basic_user_info->avatar = BuildMojoUserAvatarForUser(user);
  user_info->auth_type = auth_type;
  user_info->is_signed_in = user->is_logged_in();
  user_info->is_device_owner = is_owner;
  user_info->can_remove = CanRemoveUser(user);
  user_info->allow_fingerprint_unlock = AllowFingerprintForUser(user);
  if (!is_signin_to_add) {
    user_info->is_multiprofile_allowed = true;
  } else {
    GetMultiProfilePolicy(user, &user_info->is_multiprofile_allowed,
                          &user_info->multiprofile_policy);
  }
  if (user->GetType() == user_manager::USER_TYPE_PUBLIC_ACCOUNT) {
    user_info->public_account_info = ash::mojom::PublicAccountInfo::New();
    std::string domain;
    if (GetEnterpriseDomain(&domain))
      user_info->public_account_info->enterprise_domain = domain;
    std::string selected_locale;
    bool has_multiple_locales;
    std::unique_ptr<base::ListValue> available_locales =
        GetPublicSessionLocales(public_session_recommended_locales,
                                &selected_locale, &has_multiple_locales);
    DCHECK(available_locales);
    user_info->public_account_info->available_locales =
        lock_screen_utils::FromListValueToLocaleItem(
            std::move(available_locales));
    user_info->public_account_info->default_locale = selected_locale;
    user_info->public_account_info->show_advanced_view = has_multiple_locales;
  }
}
