 void UserSelectionScreen::FillUserDictionary(
    const user_manager::User* user,
     bool is_owner,
     bool is_signin_to_add,
     proximity_auth::mojom::AuthType auth_type,
    const std::vector<std::string>* public_session_recommended_locales,
    base::DictionaryValue* user_dict) {
  const bool is_public_session =
      user->GetType() == user_manager::USER_TYPE_PUBLIC_ACCOUNT;
  const bool is_legacy_supervised_user =
      user->GetType() == user_manager::USER_TYPE_SUPERVISED;
  const bool is_child_user = user->GetType() == user_manager::USER_TYPE_CHILD;

  user_dict->SetString(kKeyUsername, user->GetAccountId().Serialize());
  user_dict->SetString(kKeyEmailAddress, user->display_email());
  user_dict->SetString(kKeyDisplayName, user->GetDisplayName());
  user_dict->SetBoolean(kKeyPublicAccount, is_public_session);
  user_dict->SetBoolean(kKeyLegacySupervisedUser, is_legacy_supervised_user);
  user_dict->SetBoolean(kKeyChildUser, is_child_user);
  user_dict->SetBoolean(kKeyDesktopUser, false);
  user_dict->SetInteger(kKeyInitialAuthType, static_cast<int>(auth_type));
  user_dict->SetBoolean(kKeySignedIn, user->is_logged_in());
  user_dict->SetBoolean(kKeyIsOwner, is_owner);
  user_dict->SetBoolean(kKeyIsActiveDirectory, user->IsActiveDirectoryUser());
  user_dict->SetBoolean(kKeyAllowFingerprint, AllowFingerprintForUser(user));

  FillMultiProfileUserPrefs(user, user_dict, is_signin_to_add);

  if (is_public_session) {
    AddPublicSessionDetailsToUserDictionaryEntry(
        user_dict, public_session_recommended_locales);
  }
}
