UserSelectionScreen::UpdateAndReturnUserListForMojo() {
  std::vector<ash::mojom::LoginUserInfoPtr> user_info_list;

  const AccountId owner = GetOwnerAccountId();
  const bool is_signin_to_add = IsSigninToAdd();
  users_to_send_ = PrepareUserListForSending(users_, owner, is_signin_to_add);
 
   user_auth_type_map_.clear();
 
  for (user_manager::UserList::const_iterator it = users_to_send_.begin();
       it != users_to_send_.end(); ++it) {
    const AccountId& account_id = (*it)->GetAccountId();
     bool is_owner = owner == account_id;
     const bool is_public_account =
        ((*it)->GetType() == user_manager::USER_TYPE_PUBLIC_ACCOUNT);
     const proximity_auth::mojom::AuthType initial_auth_type =
         is_public_account
             ? proximity_auth::mojom::AuthType::EXPAND_THEN_USER_CLICK
            : (ShouldForceOnlineSignIn(*it)
                    ? proximity_auth::mojom::AuthType::ONLINE_SIGN_IN
                    : proximity_auth::mojom::AuthType::OFFLINE_PASSWORD);
     user_auth_type_map_[account_id] = initial_auth_type;
 
    ash::mojom::LoginUserInfoPtr login_user_info =
        ash::mojom::LoginUserInfo::New();
    const std::vector<std::string>* public_session_recommended_locales =
        public_session_recommended_locales_.find(account_id) ==
                public_session_recommended_locales_.end()
            ? nullptr
            : &public_session_recommended_locales_[account_id];
    FillUserMojoStruct(*it, is_owner, is_signin_to_add, initial_auth_type,
                       public_session_recommended_locales,
                       login_user_info.get());
    login_user_info->can_remove = CanRemoveUser(*it);
 
     if (is_public_account && LoginScreenClient::HasInstance()) {
       LoginScreenClient::Get()->RequestPublicSessionKeyboardLayouts(
          account_id, login_user_info->public_account_info->default_locale);
     }
 
    user_info_list.push_back(std::move(login_user_info));
   }
 
   return user_info_list;
}
