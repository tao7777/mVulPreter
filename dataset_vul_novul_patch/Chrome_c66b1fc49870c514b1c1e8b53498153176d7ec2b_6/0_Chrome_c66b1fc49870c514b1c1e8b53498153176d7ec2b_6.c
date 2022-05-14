UserSelectionScreen::UpdateAndReturnUserListForMojo() {
  std::vector<ash::mojom::LoginUserInfoPtr> user_info_list;

  const AccountId owner = GetOwnerAccountId();
  const bool is_signin_to_add = IsSigninToAdd();
  users_to_send_ = PrepareUserListForSending(users_, owner, is_signin_to_add);
 
   user_auth_type_map_.clear();
 
  for (const user_manager::User* user : users_to_send_) {
    const AccountId& account_id = user->GetAccountId();
     bool is_owner = owner == account_id;
     const bool is_public_account =
        user->GetType() == user_manager::USER_TYPE_PUBLIC_ACCOUNT;
     const proximity_auth::mojom::AuthType initial_auth_type =
         is_public_account
             ? proximity_auth::mojom::AuthType::EXPAND_THEN_USER_CLICK
            : (ShouldForceOnlineSignIn(user)
                    ? proximity_auth::mojom::AuthType::ONLINE_SIGN_IN
                    : proximity_auth::mojom::AuthType::OFFLINE_PASSWORD);
     user_auth_type_map_[account_id] = initial_auth_type;
 
    ash::mojom::LoginUserInfoPtr user_info = ash::mojom::LoginUserInfo::New();
    user_info->basic_user_info = ash::mojom::UserInfo::New();
    user_info->basic_user_info->type = user->GetType();
    user_info->basic_user_info->account_id = user->GetAccountId();
    user_info->basic_user_info->display_name =
        base::UTF16ToUTF8(user->GetDisplayName());
    user_info->basic_user_info->display_email = user->display_email();
    user_info->basic_user_info->avatar = BuildMojoUserAvatarForUser(user);
    user_info->auth_type = initial_auth_type;
    user_info->is_signed_in = user->is_logged_in();
    user_info->is_device_owner = is_owner;
    user_info->can_remove = CanRemoveUser(user);
    user_info->allow_fingerprint_unlock = AllowFingerprintForUser(user);

    // Fill multi-profile data.
    if (!is_signin_to_add) {
      user_info->is_multiprofile_allowed = true;
    } else {
      GetMultiProfilePolicy(user, &user_info->is_multiprofile_allowed,
                            &user_info->multiprofile_policy);
    }

    // Fill public session data.
    if (user->GetType() == user_manager::USER_TYPE_PUBLIC_ACCOUNT) {
      user_info->public_account_info = ash::mojom::PublicAccountInfo::New();
      std::string domain;
      if (GetEnterpriseDomain(&domain))
        user_info->public_account_info->enterprise_domain = domain;

      const std::vector<std::string>* public_session_recommended_locales =
          public_session_recommended_locales_.find(account_id) ==
                  public_session_recommended_locales_.end()
              ? nullptr
              : &public_session_recommended_locales_[account_id];
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

    user_info->can_remove = CanRemoveUser(user);
 
     if (is_public_account && LoginScreenClient::HasInstance()) {
       LoginScreenClient::Get()->RequestPublicSessionKeyboardLayouts(
          account_id, user_info->public_account_info->default_locale);
     }
 
    user_info_list.push_back(std::move(user_info));
   }
 
   return user_info_list;
}
