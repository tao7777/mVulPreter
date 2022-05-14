 void UserSelectionScreen::OnBeforeUserRemoved(const AccountId& account_id) {
  for (auto it = users_.cbegin(); it != users_.cend(); ++it) {
     if ((*it)->GetAccountId() == account_id) {
       users_.erase(it);
       break;
    }
  }
}
