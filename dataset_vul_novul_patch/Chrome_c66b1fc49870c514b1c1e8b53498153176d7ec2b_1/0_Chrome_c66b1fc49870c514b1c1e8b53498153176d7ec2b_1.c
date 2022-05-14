 void UserSelectionScreen::FillMultiProfileUserPrefs(
    const user_manager::User* user,
     base::DictionaryValue* user_dict,
     bool is_signin_to_add) {
   if (!is_signin_to_add) {
    user_dict->SetBoolean(kKeyMultiProfilesAllowed, true);
    return;
  }

  bool is_user_allowed;
  ash::mojom::MultiProfileUserBehavior policy;
  GetMultiProfilePolicy(user, &is_user_allowed, &policy);
  user_dict->SetBoolean(kKeyMultiProfilesAllowed, is_user_allowed);
  user_dict->SetInteger(kKeyMultiProfilesPolicy, static_cast<int>(policy));
}
