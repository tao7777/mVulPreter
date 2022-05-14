UserSelectionScreen::UpdateAndReturnUserListForWebUI() {
  std::unique_ptr<base::ListValue> users_list =
      std::make_unique<base::ListValue>();

  const AccountId owner = GetOwnerAccountId();
  const bool is_signin_to_add = IsSigninToAdd();

  users_to_send_ = PrepareUserListForSending(users_, owner, is_signin_to_add);
 
   user_auth_type_map_.clear();
 
  for (const user_manager::User* user : users_to_send_) {
    const AccountId& account_id = user->GetAccountId();
     bool is_owner = (account_id == owner);
     const bool is_public_account =
        user->GetType() == user_manager::USER_TYPE_PUBLIC_ACCOUNT;
     const proximity_auth::mojom::AuthType initial_auth_type =
         is_public_account
             ? proximity_auth::mojom::AuthType::EXPAND_THEN_USER_CLICK
            : (ShouldForceOnlineSignIn(user)
                    ? proximity_auth::mojom::AuthType::ONLINE_SIGN_IN
                    : proximity_auth::mojom::AuthType::OFFLINE_PASSWORD);
     user_auth_type_map_[account_id] = initial_auth_type;

    auto user_dict = std::make_unique<base::DictionaryValue>();
    const std::vector<std::string>* public_session_recommended_locales =
        public_session_recommended_locales_.find(account_id) ==
                 public_session_recommended_locales_.end()
             ? nullptr
             : &public_session_recommended_locales_[account_id];
    FillUserDictionary(user, is_owner, is_signin_to_add, initial_auth_type,
                        public_session_recommended_locales, user_dict.get());
    user_dict->SetBoolean(kKeyCanRemove, CanRemoveUser(user));
     users_list->Append(std::move(user_dict));
   }
 
  return users_list;
}
