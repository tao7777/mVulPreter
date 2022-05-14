void LockContentsView::OnUsersChanged(
    const std::vector<mojom::LoginUserInfoPtr>& users) {
  main_view_->RemoveAllChildViews(true /*delete_children*/);
  opt_secondary_big_view_ = nullptr;
  users_list_ = nullptr;
  rotation_actions_.clear();
  users_.clear();

  if (users.empty()) {
    LOG_IF(FATAL, screen_type_ != LockScreen::ScreenType::kLogin)
        << "Empty user list received";
    Shell::Get()->login_screen_controller()->ShowGaiaSignin(
        false /*can_close*/, base::nullopt /*prefilled_account*/);
    return;
   }
 
  for (const mojom::LoginUserInfoPtr& user : users) {
    UserState state(user->basic_user_info->account_id);
    state.fingerprint_state = user->allow_fingerprint_unlock
                                  ? mojom::FingerprintUnlockState::AVAILABLE
                                  : mojom::FingerprintUnlockState::UNAVAILABLE;
    users_.push_back(std::move(state));
  }
 
   auto box_layout =
       std::make_unique<views::BoxLayout>(views::BoxLayout::kHorizontal);
  main_layout_ = box_layout.get();
  main_layout_->set_main_axis_alignment(
      views::BoxLayout::MAIN_AXIS_ALIGNMENT_CENTER);
  main_layout_->set_cross_axis_alignment(
      views::BoxLayout::CROSS_AXIS_ALIGNMENT_CENTER);
  main_view_->SetLayoutManager(std::move(box_layout));

  primary_big_view_ = AllocateLoginBigUserView(users[0], true /*is_primary*/);
  main_view_->AddChildView(primary_big_view_);

  if (users.size() == 2)
    CreateLowDensityLayout(users);
  else if (users.size() >= 3 && users.size() <= 6)
    CreateMediumDensityLayout(users);
  else if (users.size() >= 7)
    CreateHighDensityLayout(users);

  LayoutAuth(primary_big_view_, opt_secondary_big_view_, false /*animate*/);

  OnBigUserChanged();

  PreferredSizeChanged();
  Layout();
}
