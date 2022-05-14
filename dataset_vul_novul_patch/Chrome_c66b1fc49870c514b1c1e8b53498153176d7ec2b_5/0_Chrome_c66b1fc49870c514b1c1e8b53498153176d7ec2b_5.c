const user_manager::UserList UserSelectionScreen::PrepareUserListForSending(
    const user_manager::UserList& users,
    const AccountId& owner,
    bool is_signin_to_add) {
  user_manager::UserList users_to_send;
  bool has_owner = owner.is_valid();
   size_t max_non_owner_users = has_owner ? kMaxUsers - 1 : kMaxUsers;
   size_t non_owner_count = 0;
 
  for (user_manager::User* user : users) {
    bool is_owner = user->GetAccountId() == owner;
     bool is_public_account =
        user->GetType() == user_manager::USER_TYPE_PUBLIC_ACCOUNT;
 
     if ((is_public_account && !is_signin_to_add) || is_owner ||
         (!is_public_account && non_owner_count < max_non_owner_users)) {
       if (!is_owner)
         ++non_owner_count;
       if (is_owner && users_to_send.size() > kMaxUsers) {
        users_to_send.insert(users_to_send.begin() + (kMaxUsers - 1), user);
         while (users_to_send.size() > kMaxUsers)
           users_to_send.erase(users_to_send.begin() + kMaxUsers);
       } else if (users_to_send.size() < kMaxUsers) {
        users_to_send.push_back(user);
       }
     }
   }
  return users_to_send;
}
